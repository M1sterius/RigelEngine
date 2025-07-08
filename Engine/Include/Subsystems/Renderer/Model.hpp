#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "Texture.hpp"

#include <filesystem>
#include <memory>
#include <utility>

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
        class NodeIterator
        {
        public:
            explicit NodeIterator(std::shared_ptr<Backend::Node> node)
                : m_RootNode(std::move(node))
            {
                TraverseNode(m_RootNode);
            }

            const Backend::Node* operator -> () const { return m_Nodes[m_CurrentIndex].get(); }

            NodeIterator& operator ++ ()
            {
                ++m_CurrentIndex;
                return *this;
            }

            NODISCARD bool Valid() const
            {
                return m_CurrentIndex < m_Nodes.size();
            }
        private:
            void TraverseNode(const std::shared_ptr<Backend::Node>& node)
            {
                m_Nodes.push_back(node);

                for (const auto& child : node->Children)
                    TraverseNode(child);
            }

            uint32_t m_CurrentIndex = 0;
            std::shared_ptr<Backend::Node> m_RootNode;
            std::vector<std::shared_ptr<Backend::Node>> m_Nodes;
        };

        ~Model() override;

        NODISCARD NodeIterator GetNodeIterator() const { return NodeIterator(m_RootNode); }
    INTERNAL:
        NODISCARD Ref<Backend::Vulkan::VK_VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer.get(); }
        NODISCARD Ref<Backend::Vulkan::VK_IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer.get(); }
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
