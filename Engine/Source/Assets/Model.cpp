#include "Assets/Model.hpp"
#include "Vertex.hpp"
#include "VK_VertexBuffer.hpp"
#include "VK_IndexBuffer.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

namespace Rigel
{
    using namespace Backend::Vulkan;

    Model::Model(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        Assimp::Importer importer;
        const auto scene = importer.ReadFile(m_Path.string(), aiProcess_Triangulate);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::Error("Assimp error: {}", importer.GetErrorString());
            return ErrorCode::FAILED_TO_OPEN_FILE;
        }

        // Preprocess all materials to take advantage of async loading
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
            ProcessMaterial(scene->mMaterials[i]);

        auto vertices = std::vector<Vertex>();
        auto indices = std::vector<uint32_t>();

        m_RootNode = std::make_shared<Node>();
        ProcessAiNode(scene->mRootNode, scene, m_RootNode, vertices, indices);

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(indices);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    void Model::ProcessAiNode(const aiNode* node, const aiScene* scene, const std::shared_ptr<Node>& curNode,
        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
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

    Model::Mesh Model::ProcessMesh(const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        const auto numVertices = mesh->mNumVertices;
        const auto numIndices = mesh->mNumFaces * 3; // aiProcess_Triangulate guarantees that each face is a triangle

        auto resMesh = Mesh();
        resMesh.Name = mesh->mName.C_Str();
        resMesh.FirstVertex = vertices.size();
        resMesh.VertexCount = numVertices;
        resMesh.FirstIndex = indices.size();
        resMesh.IndexCount = numIndices;

        Vertex vertex{};
        vertices.reserve(vertices.size() + numVertices);

        for (uint32_t i = 0; i < numVertices; ++i)
        {
            vertex.position = ConvertVec3(mesh->mVertices[i]);

            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            }
            else
            {
                vertex.texCoords = glm::vec2(0.0f);
            }

            vertex.normal = ConvertVec3(mesh->mNormals[i]);

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

    void Model::ProcessMaterial(const aiMaterial* aiMaterial)
    {
        ASSERT(aiMaterial, "aiMaterial was a nullptr!");

        const auto materialName = m_Path / std::format("Material{}", m_Materials.size());
        const auto texturesDir = m_Path.parent_path(); // trim to the last '/'
        auto metadata = MaterialMetadata();

        bool twoSided = false;
        if (aiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided) == AI_SUCCESS) {
            if (twoSided) {
                Debug::Message("Two sided");
            }
        }

        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString diffuseTextureName;
            aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTextureName);

            metadata.DiffusePath = texturesDir / diffuseTextureName.C_Str();
        }

        if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString specularTextureName;
            aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTextureName);

            metadata.SpecularPath = texturesDir / specularTextureName.C_Str();
        }

        if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString normalsTextureName;
            aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalsTextureName);

            metadata.NormalsPath = texturesDir / normalsTextureName.C_Str();
        }

        m_Materials.push_back(GetAssetManager()->LoadAsync<Material>(materialName, &metadata));
    }
}
