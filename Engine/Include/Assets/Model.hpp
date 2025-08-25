#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Material.hpp"

#include <filesystem>
#include <memory>
#include <utility>

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_VertexBuffer;
        class VK_IndexBuffer;

        struct Vertex3p2t3n;
    }

    /**
     * Asset that represents complex multi-mesh and multi-material model (or scene)
     */
    class Model final : public RigelAsset
    {
    INTERNAL:
        struct Mesh
        {
            std::string Name;
            AssetHandle<Material> Material;

            uint32_t FirstVertex = 0;
            uint32_t VertexCount = 0;

            uint32_t FirstIndex = 0;
            uint32_t IndexCount = 0;
        };

        struct Node
        {
            std::string Name;
            glm::mat4 Transform; // TODO: transform is relative to parent!

            std::vector<Mesh> Meshes;

            std::shared_ptr<Node> Parent;
            std::vector<std::shared_ptr<Node>> Children;
        };
    public:
        class NodeIterator
        {
        public:
            explicit NodeIterator(std::shared_ptr<Node> node)
                : m_RootNode(std::move(node))
            {
                TraverseNode(m_RootNode);
            }

            const Node* operator -> () const { return m_Nodes[m_CurrentIndex].get(); }

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
            void TraverseNode(const std::shared_ptr<Node>& node)
            {
                m_Nodes.push_back(node);

                for (const auto& child : node->Children)
                    TraverseNode(child);
            }

            uint32_t m_CurrentIndex = 0;
            std::shared_ptr<Node> m_RootNode;
            std::vector<std::shared_ptr<Node>> m_Nodes;
        };

        ~Model() override;

        NODISCARD NodeIterator GetNodeIterator() const { return NodeIterator(m_RootNode); }
    INTERNAL:
        NODISCARD Ref<Backend::Vulkan::VK_VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer.get(); }
        NODISCARD Ref<Backend::Vulkan::VK_IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer.get(); }
    private:
        Model(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        void ProcessAiNode(const aiNode* node, const aiScene* scene, const std::shared_ptr<Node>& curNode,
            std::vector<Backend::Vulkan::Vertex3p2t3n>& vertices, std::vector<uint32_t>& indices);
        Mesh ProcessMesh(const aiMesh* mesh, std::vector<Backend::Vulkan::Vertex3p2t3n>& vertices, std::vector<uint32_t>& indices);

        AssetHandle<Material> ProcessMaterial(const aiMaterial* aiMaterial);

        std::unique_ptr<Backend::Vulkan::VK_VertexBuffer> m_VertexBuffer;
        std::unique_ptr<Backend::Vulkan::VK_IndexBuffer> m_IndexBuffer;

        std::shared_ptr<Node> m_RootNode;
        std::vector<AssetHandle<Material>> m_Materials;

        friend class AssetManager;
    };
}
