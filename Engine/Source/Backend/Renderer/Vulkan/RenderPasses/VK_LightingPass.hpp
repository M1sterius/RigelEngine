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
    class VK_ImGUI_Renderer;

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
    class VK_GPUScene;

    struct AcquireImageInfo;

    class VK_LightingPass 
    {
    public:
        VK_LightingPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer, VK_GPUScene& gpuScene);
        ~VK_LightingPass();

        VK_LightingPass(const VK_LightingPass&) = delete;
        VK_LightingPass operator = (const VK_LightingPass&) = delete;

        void Recreate();
        void SetImGuiBackend(VK_ImGUI_Renderer* backend) { m_ImGuiBackend = backend; }

        NODISCARD VkCommandBuffer RecordCommandBuffer(const AcquireImageInfo& swapchainImage, const uint32_t frameIndex);
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;
        VK_GBuffer& m_GBuffer;
        VK_GPUScene& m_GPUScene;

        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;
        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;

        VkSampler m_GBufferSampler = VK_NULL_HANDLE;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;

        void CreateSampler();
        void CreateDescriptorSet();
        void CreateGraphicsPipeline();
    };
}
