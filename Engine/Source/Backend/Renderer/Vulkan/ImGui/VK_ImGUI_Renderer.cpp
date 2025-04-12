#include "VK_ImGUI_Renderer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Config.hpp"
#include "VulkanWrapper.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"
#include "imgui/imgui.h"
#include "imgui/vulkan/imgui_impl_vulkan.h"
#include "imgui/vulkan/imgui_impl_glfw.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    VK_ImGUI_Renderer::VK_ImGUI_Renderer(VK_Renderer& renderer)
        : m_Renderer(renderer)
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        const auto& windowManager = Engine::Get().GetWindowManager();
        ImGui_ImplGlfw_InitForVulkan(windowManager.GetGLFWWindowPtr(), true);

        const std::vector<VkDescriptorPoolSize> poolSizes = {{
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        }};

        auto& device = m_Renderer.GetDevice();
        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(device, poolSizes, 1000);

        VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo {};
        pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.ApiVersion = VK_Config::MinimalRequiredVulkanVersion;
        init_info.Instance = m_Renderer.GetInstance().Get();
        init_info.PhysicalDevice = device.GetPhysicalDevice();
        init_info.Device = device.Get();
        init_info.QueueFamily = device.GetQueueFamilyIndices().GraphicsFamily.value();
        init_info.Queue = device.GetGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.PipelineRenderingCreateInfo = pipelineRenderingInfo;
        init_info.DescriptorPool = m_DescriptorPool->Get();
        init_info.Subpass = 0;
        init_info.MinImageCount = m_Renderer.GetSwapchain().GetFramesInFlightCount() - 1; // maybe it's not a good idea?
        init_info.ImageCount = m_Renderer.GetSwapchain().GetFramesInFlightCount();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.UseDynamicRendering = true;

        ImGui_ImplVulkan_Init(&init_info);
    }

    VK_ImGUI_Renderer::~VK_ImGUI_Renderer()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void VK_ImGUI_Renderer::RenderFrame(const VK_CmdBuffer& cmdBuffer)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer.Get());
    }
}
