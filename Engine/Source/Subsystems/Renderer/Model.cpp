#include "Model.hpp"
#include "Renderer.hpp"
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

        m_RootNode = std::make_shared<Backend::Node>();
        ProcessAiNode(scene->mRootNode, scene, m_RootNode);

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(GetDevice(), m_Vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(GetDevice(), m_Indices);

        // This little trick completely clears vectors and deallocates their memory!
        std::vector<Vertex>().swap(m_Vertices);
        std::vector<uint32_t>().swap(m_Indices);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    void Model::ProcessAiNode(const aiNode* node, const aiScene* scene, std::shared_ptr<Backend::Node> curNode)
    {
        curNode->Name = node->mName.C_Str();
        curNode->Transform = ConvertMat4(node->mTransformation);

        curNode->Meshes.reserve(node->mNumMeshes);
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            const auto mesh = scene->mMeshes[node->mMeshes[i]];
            curNode->Meshes.emplace_back(ProcessMesh(mesh, scene));
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            auto childNode = std::make_shared<Backend::Node>();

            childNode->Parent = curNode;
            curNode->Children.push_back(childNode);

            ProcessAiNode(node->mChildren[i], scene, childNode);
        }
    }

    Backend::Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
    {
        const auto numVertices = mesh->mNumVertices;
        const auto numIndices = mesh->mNumFaces * 3; // aiProcess_Triangulate guarantees that each face is a triangle

        auto resMesh = Backend::Mesh();
        resMesh.FirstVertex = m_Vertices.size();
        resMesh.VertexCount = numVertices;
        resMesh.FirstIndex = m_Indices.size();
        resMesh.IndexCount = numIndices;

        Vertex vertex{};
        m_Vertices.reserve(m_Vertices.size() + numVertices);

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

            m_Vertices.push_back(vertex);
        }

        m_Indices.reserve(m_Indices.size() + numIndices);

        for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            const auto face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; ++j)
                m_Indices.push_back(face.mIndices[j]);
        }

        return resMesh;
    }
}
