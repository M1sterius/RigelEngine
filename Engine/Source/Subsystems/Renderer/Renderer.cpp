#include "Renderer.hpp"
#include "Debug.hpp"

#include "Renderer/Vulkan/VK_Renderer.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }
    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        RGE_TRACE("Starting up renderer.");

        m_SelectedGraphicsAPI = SelectGraphicsAPI();

        if (m_SelectedGraphicsAPI == GraphicsApi::None)
            Debug::ThrowError("This device does not support any of the available graphics APIs!");

        if (m_SelectedGraphicsAPI == GraphicsApi::Vulkan)
        {
            RGE_TRACE("Selected Vulkan graphics API.");
            m_BackendRenderer = std::make_unique<backend::VK_Renderer>();
        }
    }

    void Renderer::Shutdown()
    {
        RGE_TRACE("Shutting down renderer.");
    }

    GraphicsApi Renderer::SelectGraphicsAPI()
    {
        // For now only vulkan is supported
        return GraphicsApi::Vulkan;
    }
}
