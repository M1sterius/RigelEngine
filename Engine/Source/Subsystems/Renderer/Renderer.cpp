#include "Renderer.hpp"
#include "Logger.hpp"

#include "Renderer/Vulkan/VK_Renderer.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }
    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        RGE_TRACE("Starting up renderer.");

        // For now only vulkan renderer is supported
        m_VKRenderer = std::make_unique<backend::VK_Renderer>();
    }

    void Renderer::Shutdown()
    {
        RGE_TRACE("Shutting down renderer.");
    }
}
