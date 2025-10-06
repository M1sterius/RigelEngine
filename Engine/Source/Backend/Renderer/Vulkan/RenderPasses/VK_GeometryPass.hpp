#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel::Backend
{
    struct SceneRenderInfo;
}

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_GBuffer;
    class VK_GraphicsPipeline;
    class VK_CmdBuffer;
    class VK_MemoryBuffer;
    class VK_VertexBuffer;
    class VK_IndexBuffer;
    class VK_DescriptorPool;
    class VK_Swapchain;
    class VK_BindlessManager;

    struct MeshData;

    struct BakedDrawCall
    {
        uint32_t MeshIndex;

        uint32_t IndexCount;
        uint32_t FirstIndex;
        int32_t VertexOffset;
    };

    struct BakedDrawBatch
    {
        Ref<VK_VertexBuffer> VertexBuffer;
        Ref<VK_IndexBuffer> IndexBuffer;

        std::vector<BakedDrawCall> DrawCalls;
    };

    class VK_GeometryPass
    {
    public:
        VK_GeometryPass(VK_Device& device, VK_Swapchain& swapchain, VK_BindlessManager& bindlessManager,
            VK_GBuffer& gBuffer);
        ~VK_GeometryPass();

        VK_GeometryPass(const VK_GeometryPass&) = delete;
        VK_GeometryPass operator = (const VK_GeometryPass&) = delete;

        void SetMeshData(const Ref<SceneRenderInfo> sceneRenderInfo, const uint32_t frameIndex);
        NODISCARD VkCommandBuffer RecordCommandBuffer(const uint32_t frameIndex);
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;
        VK_BindlessManager& m_BindlessManager;
        VK_GBuffer& m_GBuffer;

        static constexpr uint32_t MAX_MESHES = 2048;

        void CreateDescriptorSet();
        void CreateGraphicsPipeline();

        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> m_DescriptorSets;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;
        std::vector<std::unique_ptr<VK_MemoryBuffer>> m_MeshBuffers;

        std::vector<MeshData> m_Meshes;
        std::vector<BakedDrawBatch> m_DrawBatches;
    };
}
