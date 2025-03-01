#include "Renderer.hpp"
#include "Logger.hpp"

#include "Renderer/Vulkan/VK_Renderer.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }
    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        VERBOSE_MESSAGE("Starting up renderer.");

        // For now only vulkan renderer is supported
        m_VKRenderer = std::make_unique<VK_Renderer>();
    }

    void Renderer::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down renderer.");
    }
}
