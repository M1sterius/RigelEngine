#include "VK_Renderer.hpp"
#include "VK_Config.hpp"
#include "VK_Instance.hpp"
#include "Debug.hpp"

#include "vulkan.h"

namespace rge::backend
{
    VK_Renderer::VK_Renderer() { Startup(); }
    VK_Renderer::~VK_Renderer() { Shutdown(); }

    void VK_Renderer::Startup()
    {
        RGE_TRACE("Starting up Vulkan renderer!");

        RGE_TRACE("Creating vulkan instance.");
        m_Instance = std::make_unique<VK_Instance>();
    }

    void VK_Renderer::Shutdown()
    {

    }
}
