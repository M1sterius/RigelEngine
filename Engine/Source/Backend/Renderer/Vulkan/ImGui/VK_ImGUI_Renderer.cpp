#include "VK_ImGUI_Renderer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "VulkanWrapper.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"
#include "imgui/imgui.h"
#include "imgui/vulkan/imgui_impl_vulkan.h"
#include "imgui/vulkan/imgui_impl_glfw.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    VK_ImGUI_Renderer::VK_ImGUI_Renderer(VK_Renderer& renderer)
        : m_Renderer(renderer) { }
    VK_ImGUI_Renderer::~VK_ImGUI_Renderer() = default;

    ErrorCode VK_ImGUI_Renderer::Startup()
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.Fonts->AddFontDefault();

        const auto& windowManager = Engine::Get().GetWindowManager();
        ImGui_ImplGlfw_InitForVulkan(windowManager.GetGLFWWindowPtr(), true);

        const std::vector<VkDescriptorPoolSize> poolSizes = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
        };

        auto& device = m_Renderer.GetDevice();
        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(device, poolSizes, 1);

        const auto imageFormat = m_Renderer.GetSwapchain().GetSwapchainImageFormat();

        // We specify this so that imgui will enable blending
        VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo {};
        pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &imageFormat;

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.ApiVersion = VK_Config::MinimalRequiredAPIVersion;
        init_info.Instance = m_Renderer.GetInstance().Get();
        init_info.PhysicalDevice = device.GetPhysicalDevice();
        init_info.Device = device.Get();
        init_info.QueueFamily = device.GetQueueFamilyIndices().GraphicsFamily.value();
        init_info.Queue = device.GetGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.PipelineRenderingCreateInfo = pipelineRenderingInfo;
        init_info.DescriptorPool = m_DescriptorPool->Get();
        init_info.Subpass = 0;
        init_info.MinImageCount = m_Renderer.GetSwapchain().GetMinImageCount();
        init_info.ImageCount = m_Renderer.GetSwapchain().GetFramesInFlightCount();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.UseDynamicRendering = true;

        ImGui_ImplVulkan_Init(&init_info);
        ImGui_ImplVulkan_CreateFontsTexture();

        return ErrorCode::NONE;
    }

    ErrorCode VK_ImGUI_Renderer::Shutdown()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return ErrorCode::NONE;
    }

    void VK_ImGUI_Renderer::BeginNewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VK_ImGUI_Renderer::RenderFrame(void* optData)
    {
        const auto cmdBuffer = static_cast<VkCommandBuffer>(optData);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
    }
}
