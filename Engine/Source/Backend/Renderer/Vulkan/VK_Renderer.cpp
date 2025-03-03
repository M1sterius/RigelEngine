#include "VK_Renderer.hpp"
#include "VK_Config.hpp"
#include "Debug.hpp"

#include "vulkan.h"

namespace rge_bcknd
{
    VK_Renderer::VK_Renderer() { Startup(); }
    VK_Renderer::~VK_Renderer() { Shutdown(); }

    void VK_Renderer::Startup()
    {
        RGE_TRACE("Starting up Vulkan renderer!");
    }

    void VK_Renderer::Shutdown()
    {

    }
}
