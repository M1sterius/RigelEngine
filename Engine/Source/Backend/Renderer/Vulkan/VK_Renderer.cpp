#include "VK_Renderer.hpp"
#include "VK_Config.hpp"
#include "VK_Instance.hpp"
#include "VK_Surface.hpp"
#include "VK_Device.hpp"
#include "VK_Swapchain.hpp"
#include "Debug.hpp"
#include "Time.hpp"

#include "Engine.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"

namespace rge::backend
{
    VK_Renderer::VK_Renderer() { Startup(); }
    VK_Renderer::~VK_Renderer() { Shutdown(); }

    void VK_Renderer::Startup()
    {
        RGE_TRACE("Starting up Vulkan renderer.");

        const auto& windowManager = Engine::Get().GetWindowManager();

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), windowManager.GetSize());

        m_Initialized = true;
    }

    void VK_Renderer::Shutdown()
    {
        RGE_TRACE("Shutting down Vulkan renderer.");
    }

    void VK_Renderer::InitImGUI()
    {

    }

    void VK_Renderer::PrepareFrame()
    {
        m_FrameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
    }

    void VK_Renderer::RenderScene()
    {

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
