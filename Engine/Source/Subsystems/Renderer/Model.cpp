#include "Model.hpp"
#include "AssetManager.hpp"
#include "BuiltInAssets.hpp"
#include "VK_VertexBuffer.hpp"
#include "VK_IndexBuffer.hpp"
#include "VulkanUtility.hpp"

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

    inline Backend::Material GetDefaultMaterial()
    {
        auto& assetManager = Engine::Get().GetAssetManager();

        return Backend::Material{
            .Diffuse = assetManager.Load<Texture>(BuiltInAssets::TextureError),
            .Specular = assetManager.Load<Texture>(BuiltInAssets::TextureBlack),
            .Normal = assetManager.Load<Texture>(BuiltInAssets::TextureBlack)
        };
    }

    Model::Model(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        Assimp::Importer importer;
        const auto scene = importer.ReadFile(m_Path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::Error("Assimp error: {}", importer.GetErrorString());
            return ErrorCode::FAILED_TO_OPEN_FILE;
        }

        auto vertices = std::vector<Vertex>();
        auto indices = std::vector<uint32_t>();

        m_RootNode = std::make_shared<Backend::Node>();
        ProcessAiNode(scene->mRootNode, scene, m_RootNode, vertices, indices);

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(GetDevice(), vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(GetDevice(), indices);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    void Model::ProcessAiNode(const aiNode* node, const aiScene* scene, const std::shared_ptr<Backend::Node>& curNode,
        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        curNode->Name = node->mName.C_Str();
        curNode->Transform = ConvertMat4(node->mTransformation);

        curNode->Meshes.reserve(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            const auto mesh = scene->mMeshes[node->mMeshes[i]];
            curNode->Meshes.emplace_back(ProcessMesh(mesh, scene, vertices, indices));
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            auto childNode = std::make_shared<Backend::Node>();

            childNode->Parent = curNode;
            curNode->Children.push_back(childNode);

            ProcessAiNode(node->mChildren[i], scene, childNode, vertices, indices);
        }
    }

    Backend::Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene,
        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        const auto numVertices = mesh->mNumVertices;
        const auto numIndices = mesh->mNumFaces * 3; // aiProcess_Triangulate guarantees that each face is a triangle

        auto resMesh = Backend::Mesh();
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

        resMesh.Material = ProcessMaterial(mesh->mMaterialIndex, scene);

        return resMesh;
    }

    Backend::Material Model::ProcessMaterial(const uint32_t aiMaterialIndex, const aiScene* scene) const
    {
        const auto aiMaterial = scene->mMaterials[aiMaterialIndex];
        ASSERT(aiMaterial, "aiMaterial was a nullptr!");

        const auto texturesDir = m_Path.parent_path(); // trim to the last '/'
        auto& assetManager = Engine::Get().GetAssetManager();

        auto resMaterial = GetDefaultMaterial();

        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString diffuseTextureName;
            aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTextureName);

            const auto path = texturesDir / diffuseTextureName.C_Str();
            resMaterial.Diffuse = assetManager.LoadAsync<Texture>(path);
        }

        if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString specularTextureName;
            aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTextureName);

            const auto path = texturesDir / specularTextureName.C_Str();
            resMaterial.Specular = assetManager.LoadAsync<Texture>(path);
        }

        if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString normalsTextureName;
            aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalsTextureName);

            const auto path = texturesDir / normalsTextureName.C_Str();
            resMaterial.Normal = assetManager.LoadAsync<Texture>(path);
        }

        return resMaterial;
    }
}
