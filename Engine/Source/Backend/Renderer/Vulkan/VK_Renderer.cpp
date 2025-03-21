#include "VK_Renderer.hpp"

#include "VK_CmdBuffer.hpp"
#include "VK_Fence.hpp"
#include "VK_Semaphore.hpp"
#include "VK_Config.hpp"
#include "VK_Instance.hpp"
#include "VK_Surface.hpp"
#include "VK_Device.hpp"
#include "VK_Swapchain.hpp"
#include"VK_Image.hpp"
#include "Debug.hpp"
#include "Time.hpp"

#include "Engine.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"

namespace rge::backend
{
    VK_Renderer::VK_Renderer() : m_WindowManager(Engine::Get().GetWindowManager())
    { Startup(); }
    VK_Renderer::~VK_Renderer() { Shutdown(); }

    void VK_Renderer::Startup()
    {
        Debug::Trace("Starting up Vulkan renderer.");

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), m_WindowManager.GetSize());

        const auto frameInFlight = m_Swapchain->GetFramesInFlightCount();

        for (uint32_t i = 0; i < frameInFlight; i++)
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));

        for (uint32_t i = 0; i < frameInFlight; i++)
            m_RenderFinishedSemaphore.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));

        m_Initialized = true;
    }

    void VK_Renderer::Shutdown()
    {
        m_Device->WaitIdle();
        Debug::Trace("Shutting down Vulkan renderer.");
    }

    void VK_Renderer::RecreateSwapchain() const
    {
        m_WindowManager.WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = m_WindowManager.GetSize();
        const auto vsync = m_WindowManager.IsVsyncEnabled();

        m_Swapchain->SetupSwapchain(windowSize, vsync);
    }

    void VK_Renderer::InitImGUI()
    {

    }

    void VK_Renderer::PrepareFrame()
    {

    }

    void VK_Renderer::RenderScene()
    {
        /*
         *  1) Acquire an image
         *  2) Transition an image to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
         *  3) Record command buffer
         *      3.1) Begin dynamic rendering
         *      3.2) Bind the pipeline
         *      3.3) Issue draw calls
         *      3.4) Transition the image to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
         *  4) Submit command buffer
         *  5) Present the image after render is finished
         */

        if (m_WindowManager.GetWindowResizeFlag())
        {
            m_WindowManager.ResetWindowResizeFlag();
            RecreateSwapchain();
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        const auto image = m_Swapchain->AcquireNextImage();

        VK_Image::TransitionLayout(*m_Device, image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        const auto commandBuffer = VK_CmdBuffer(*m_Device);
        commandBuffer.BeginRecording(0);

        VkRenderingAttachmentInfo colorAttachment {};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = image.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.0f, 0.5f, 1.0f, 1.0f}};

        VkRenderingInfo renderingInfo {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(commandBuffer.Get(), &renderingInfo);
        vkCmdEndRendering(commandBuffer.Get());

        commandBuffer.EndRecording();

        VK_Image::TransitionLayout(*m_Device, image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        m_Swapchain->Present(image.imageIndex);
    }

    void VK_Renderer::RenderGizmo()
    {

    }

    void VK_Renderer::RenderGUI()
    {

    }

    void VK_Renderer::FinalizeFrame()
    {

    }
}
