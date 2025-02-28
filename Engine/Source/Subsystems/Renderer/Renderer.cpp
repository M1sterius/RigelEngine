#include "Renderer.hpp"
#include "Logger.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }
    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        VERBOSE_MESSAGE("Starting up renderer.");

        m_GraphicsAPI = ChooseGraphicsAPI();

        if (m_GraphicsAPI == GraphicsApi::Default)
        {
            THROW_RUNTIME_ERROR("Failed to select any of the available graphics APIs");
        }
    }

    void Renderer::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down renderer.");
    }

    GraphicsApi Renderer::ChooseGraphicsAPI() const
    {
        return GraphicsApi::Vulkan;
    }
}
