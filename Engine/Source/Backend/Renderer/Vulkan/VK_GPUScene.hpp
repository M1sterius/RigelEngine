#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel
{
    class RenderScene;
}

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Swapchain;
    class VK_MemoryBuffer;
    class VK_VertexBuffer;
    class VK_IndexBuffer;
    class VK_DescriptorSet;
    class VK_DescriptorPool;

    struct SceneData;

    class VK_GPUScene
    {
    public:
        struct DrawCall
        {
            uint32_t MeshIndex;

            uint32_t IndexCount;
            uint32_t FirstIndex;
            int32_t VertexOffset;
        };

        struct DrawBatch
        {
            Ref<VK_VertexBuffer> VertexBuffer;
            Ref<VK_IndexBuffer> IndexBuffer;

            std::vector<DrawCall> DrawCalls;
        };

        VK_GPUScene(VK_Device& device, VK_Swapchain& swapchain);
        ~VK_GPUScene();

        VK_GPUScene(const VK_GPUScene&) = delete;
        VK_GPUScene operator = (const VK_GPUScene&) = delete;

        NODISCARD VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
        NODISCARD VkDescriptorSet GetDescriptorSet(const uint32_t frameIndex) const { return m_DescriptorSets[frameIndex]; }

        NODISCARD const SceneData* GetSceneData() const { return m_SceneData.get(); }
        NODISCARD const std::vector<DrawBatch>& GetDeferredDrawBatches() const { return m_DeferredDrawBatches; }
        NODISCARD const std::vector<DrawBatch>& GetForwardDrawBatches() const { return m_ForwardDrawBatches; }

        void Update(const uint32_t frameIndex, const RenderScene& scene);
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;

        void CreateDescriptorSet();

        std::unique_ptr<SceneData> m_SceneData;

        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::vector<VkDescriptorSet> m_DescriptorSets;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;
        std::vector<std::unique_ptr<VK_MemoryBuffer>> m_Buffers;

        std::vector<DrawBatch> m_DeferredDrawBatches;
        std::vector<DrawBatch> m_ForwardDrawBatches;
    };
}
