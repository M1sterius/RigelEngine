#include "Renderer.hpp"
#include "Debug.hpp"

#include "Renderer/Vulkan/VK_Renderer.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }
    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        Debug::Trace("Starting up renderer.");

        m_SelectedGraphicsAPI = SelectGraphicsAPI();

        // For now only Vulkan is supported
        m_BackendRenderer = std::make_unique<backend::VK_Renderer>();
        m_BackendRenderer->InitImGUI();

        m_Initialized = true;
    }

    void Renderer::Shutdown()
    {
        Debug::Trace("Shutting down renderer.");
    }

    GraphicsApi Renderer::SelectGraphicsAPI()
    {
        // For now only vulkan is supported
        return GraphicsApi::Vulkan;
    }

    backend::RenderingBackend& Renderer::GetBackend() const
    {
        ASSERT(m_BackendRenderer, "Attempted to retrieve a null reference of a rendering backend!");
        return *m_BackendRenderer;
    }

    std::string Renderer::GetGraphicsAPIString(const GraphicsApi api)
    {
        switch (api)
        {
            case GraphicsApi::Vulkan: return "Vulkan";
            case GraphicsApi::OpenGL: return "OpenGL";
            default: return "None";
        }
    }

    void Renderer::LateInit() const
    {
        m_BackendRenderer->LateInit();
    }

    void Renderer::Prepare()
    {
        // Find main camera
        // Cull models
    }

    void Renderer::Render() const
    {
        m_BackendRenderer->Render();
    }
}
