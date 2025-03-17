#include "VK_Renderer.hpp"
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

        m_Initialized = true;
    }

    void VK_Renderer::Shutdown()
    {
        vkQueueWaitIdle(m_Device->GetGraphicsQueue());
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

        m_FrameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        m_SwapchainImageIndex = m_Swapchain->AcquireNextImage(m_FrameIndex);
        const auto image = m_Swapchain->GetImages()[m_SwapchainImageIndex];

        VK_Image::TransitionLayout(*m_Device, image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        // Render here!
        VK_Image::TransitionLayout(*m_Device, image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        m_Swapchain->Present(m_SwapchainImageIndex, m_FrameIndex);
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
