#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_ImGUI_Renderer;

    class VK_Renderer;
    class VK_Device;
    class VK_GBuffer;
    class VK_BindlessManager;
    class VK_GraphicsPipeline;
    class VK_CmdBuffer;
    class VK_Swapchain;
    class VK_GPUScene;

    struct AcquireImageInfo;

    class VK_ForwardPass
    {
    public:
        VK_ForwardPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer, VK_BindlessManager& bindlessManager, VK_GPUScene& gpuScene);
        ~VK_ForwardPass();

        VK_ForwardPass(const VK_ForwardPass&) = delete;
        VK_ForwardPass operator = (const VK_ForwardPass&) = delete;

        void SetImGuiBackend(VK_ImGUI_Renderer* backend) { m_ImGuiBackend = backend; }

        NODISCARD VkCommandBuffer RecordCommandBuffer(const AcquireImageInfo& swapchainImage, const uint32_t frameIndex);
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;
        VK_GBuffer& m_GBuffer;
        VK_BindlessManager& m_BindlessManager;
        VK_GPUScene& m_GPUScene;

        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;

        void CreateGraphicsPipeline();
    };
}
