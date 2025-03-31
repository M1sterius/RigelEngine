#include "Renderer.hpp"

#include "Camera.hpp"
#include "ComponentHandle.hpp"

#include "Debug.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "ModelRenderer.hpp"
#include "Renderer/Vulkan/VK_Renderer.hpp"

namespace rge
{
    namespace backend
    {
        struct RenderingBackendRenderInfo
        {
            ComponentHandle<Camera> MainCamera;

        };
    }

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
        // Find all renderable objects
        // Find all light sources

        // Cull models, dispatch OnBecomeVisible/OnBecomeInvisible

        auto scene = Engine::Get().GetSceneManager().GetLoadedScene();
        if (scene.IsNull())
        {
            Debug::Warning("No scene is currently loaded. Rendering skipped!");
            return;
        }

        // const auto models = scene->Search([](GOHandle& go) -> bool
        // {
        //     if (go->HasComponent<rge::ModelRenderer>())
        //         return true;
        //     return false;
        // });
        //
        // Debug::Message("{}", models.size());
    }

    void Renderer::Render() const
    {
        m_BackendRenderer->Render();
    }
}
