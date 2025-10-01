#include "Assets/Model.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Backend/Renderer/Vulkan/Helpers/Vertex.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VK_VertexBuffer.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VK_IndexBuffer.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf/tiny_gltf.h"

inline glm::mat4 ConvertMat4(const aiMatrix4x4& aiMat)
{
    //transposed
    return {
        aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
        aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
        aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
        aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
      };
}

inline glm::vec3 ConvertVec3(const aiVector3D& vec)
{
    return {
        vec.x, vec.y, vec.z
    };
}

inline glm::vec2 ConvertVec2(const aiVector2D& vec)
{
    return {
        vec.x, vec.y
    };
}

inline glm::vec3 ConvertColor(const aiColor3D& color)
{
    return {color.r, color.g, color.b};
}

namespace Rigel
{
    using namespace Backend::Vulkan;

    Model::Model(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        Assimp::Importer importer;
        const auto scene = importer.ReadFile(m_Path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::Error("Assimp error: {}", importer.GetErrorString());
            return ErrorCode::FAILED_TO_OPEN_FILE;
        }

        // Preprocess all materials to take advantage of async loading
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
            m_Materials.emplace_back(ProcessMaterial(scene->mMaterials[i]));

        auto vertices = std::vector<Vertex3p2t3n4g>();
        auto indices = std::vector<uint32_t>();

        m_RootNode = std::make_shared<Node>();
        ProcessAiNode(scene->mRootNode, scene, m_RootNode, vertices, indices);

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(indices);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    void Model::ProcessAiNode(const aiNode* node, const aiScene* scene, const std::shared_ptr<Node>& curNode,
        std::vector<Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices)
    {
        curNode->Name = node->mName.C_Str();
        curNode->Transform = ConvertMat4(node->mTransformation);

        curNode->Meshes.reserve(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            const auto mesh = scene->mMeshes[node->mMeshes[i]];
            curNode->Meshes.emplace_back(ProcessMesh(mesh, vertices, indices));
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            auto childNode = std::make_shared<Node>();

            childNode->Parent = curNode;
            curNode->Children.push_back(childNode);

            ProcessAiNode(node->mChildren[i], scene, childNode, vertices, indices);
        }
    }

    Model::Mesh Model::ProcessMesh(const aiMesh* mesh, std::vector<Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices)
    {
        const auto numVertices = mesh->mNumVertices;
        const auto numIndices = mesh->mNumFaces * 3; // aiProcess_Triangulate guarantees that each face is a triangle

        auto resMesh = Mesh();
        resMesh.Name = mesh->mName.C_Str();
        resMesh.FirstVertex = vertices.size();
        resMesh.VertexCount = numVertices;
        resMesh.FirstIndex = indices.size();
        resMesh.IndexCount = numIndices;

        Vertex3p2t3n4g vertex{};
        vertices.reserve(vertices.size() + numVertices);

        for (uint32_t i = 0; i < numVertices; ++i)
        {
            vertex.Position = ConvertVec3(mesh->mVertices[i]);

            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            }

            vertex.Normal = ConvertVec3(mesh->mNormals[i]);

            if (mesh->mTangents && mesh->mBitangents)
            {
                const auto T = ConvertVec3(mesh->mTangents[i]);
                const auto B = ConvertVec3(mesh->mBitangents[i]);
                const auto N = vertex.Normal;

                const auto w = (glm::dot(glm::cross(N, T), B) < 0.0) ? -1.0f : 1.0f;

                vertex.Tangent = glm::vec4(T, w);
            }

            vertices.push_back(vertex);
        }

        indices.reserve(indices.size() + numIndices);

        for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            const auto face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        const auto& material = m_Materials[mesh->mMaterialIndex];
        material->WaitReady();
        resMesh.Material = material;

        return resMesh;
    }

    AssetHandle<Material> Model::ProcessMaterial(const aiMaterial* aiMaterial)
    {
        ASSERT(aiMaterial, "aiMaterial was a nullptr!");

        const auto materialName = m_Path / std::format("Material{}", m_Materials.size());
        const auto texturesDir = m_Path.parent_path(); // trim to the last '/'

        auto metadata = MaterialMetadata();
        // TODO: Update assimp to get correct roughness and metallic textures!

        // Albedo
        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString albedoTextureName;
            aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &albedoTextureName);

            metadata.AlbedoTex = texturesDir / albedoTextureName.C_Str();
        }
        else
        {
            aiColor3D color{};
            if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                metadata.Color = ConvertColor(color);
        }

        // Metallic
        if (aiMaterial->GetTextureCount(aiTextureType_METALNESS) > 0)
        {
            aiString metallicTextureName;
            aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &metallicTextureName);

            metadata.MetallicTex = texturesDir / metallicTextureName.C_Str();
        }
        else
        {
            float32_t metalness{0.0};
            if (aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metalness) == AI_SUCCESS)
                metadata.Metalness = metalness;
        }

        // Roughness
        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
        {
            aiString roughnessTextureName;
            aiMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessTextureName);

            metadata.RoughnessTex = texturesDir / roughnessTextureName.C_Str();
        }
        else
        {
            float32_t roughness{1.0};
            if (aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS)
                metadata.Roughness = roughness;
        }

        // Normal
        if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString normalsTextureName;
            aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalsTextureName);

            metadata.NormalTex = texturesDir / normalsTextureName.C_Str();
        }

        // Ambient occlusion
        if (aiMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            aiString ambientOcclusionTextureName;
            aiMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &ambientOcclusionTextureName);

            metadata.AmbientOcclusionTex = texturesDir / ambientOcclusionTextureName.C_Str();
        }

        bool twoSided{false};
        if (aiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided) == AI_SUCCESS)
            metadata.TwoSided = twoSided;

        return GetAssetManager()->LoadAsync<Material>(materialName, &metadata);
    }
}
