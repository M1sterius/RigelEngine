#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "Texture.hpp"

#include <filesystem>
#include <memory>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Rigel
{
    namespace Backend
    {
        namespace Vulkan
        {
            class VK_VertexBuffer;
            class VK_IndexBuffer;

            struct Vertex;
        }

        struct Material
        {
            AssetHandle<Texture> Diffuse;
            AssetHandle<Texture> Specular;
            AssetHandle<Texture> Normal;
        };

        struct Mesh
        {
            std::string Name;

            uint32_t FirstVertex{0};
            uint32_t VertexCount{0};

            uint32_t FirstIndex{0};
            uint32_t IndexCount{0};

            Material Material;
        };

        struct Node
        {
            std::string Name;
            glm::mat4 Transform;

            std::vector<Mesh> Meshes;

            std::shared_ptr<Node> Parent;
            std::vector<std::shared_ptr<Node>> Children;
        };
    }

    class Model final : public RigelAsset
    {
    public:
        ~Model() override;
    INTERNAL:
    private:
        Model(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        void ProcessAiNode(const aiNode* node, const aiScene* scene, std::shared_ptr<Backend::Node> curNode);
        Backend::Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
        Backend::Material ProcessMaterial(const uint32_t aiMaterialIndex, const aiScene* scene) const;

        std::unique_ptr<Backend::Vulkan::VK_VertexBuffer> m_VertexBuffer;
        std::unique_ptr<Backend::Vulkan::VK_IndexBuffer> m_IndexBuffer;

        // these buffers are used only during the loading process and cleaned up right after uploading the data to the GPU
        std::vector<Backend::Vulkan::Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        std::shared_ptr<Backend::Node> m_RootNode;

        friend class AssetManager;
    };
}
