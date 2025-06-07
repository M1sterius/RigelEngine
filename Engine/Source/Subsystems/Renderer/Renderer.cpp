#include "Renderer.hpp"

#include "Camera.hpp"
#include "ComponentHandle.hpp"

#include "Debug.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "ModelRenderer.hpp"
#include "Renderer/Vulkan/VK_Renderer.hpp"
#include "Renderer/Vulkan/ImGui/VK_ImGUI_Renderer.hpp"

namespace Rigel
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    ErrorCode Renderer::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up renderer.");

        m_SelectedGraphicsAPI = SelectGraphicsAPI();

        // For now only Vulkan is supported
        m_BackendRenderer = std::make_unique<Backend::Vulkan::VK_Renderer>();
        m_ImGuiBackend = std::make_unique<Backend::Vulkan::VK_ImGUI_Renderer>(dynamic_cast<Backend::Vulkan::VK_Renderer&>(*m_BackendRenderer));

        auto& rendererBackend = dynamic_cast<Backend::Vulkan::VK_Renderer&>(*m_BackendRenderer);
        const auto ImGuiBackend = dynamic_cast<Backend::Vulkan::VK_ImGUI_Renderer*>(m_ImGuiBackend.get());

        rendererBackend.SetImGuiBackend(ImGuiBackend);

        m_Initialized = true;
        return ErrorCode::NONE;
    }

    ErrorCode Renderer::Shutdown()
    {
        Debug::Trace("Shutting down renderer.");
        return ErrorCode::NONE;
    }

    Backend::IRendererBackend& Renderer::GetBackend() const
    {
        ASSERT(m_BackendRenderer, "Attempted to retrieve a null reference of a rendering backend!");
        return *m_BackendRenderer;
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

        m_CurrentRenderInfo.Reset();

        auto scene = Engine::Get().GetSceneManager().GetLoadedScene();
        if (scene.IsNull()) return;

        auto cameras = scene->Search([](GOHandle& go) -> bool
        {
            if (go->HasComponent<Camera>())
                return true;
            return false;
        });

        if (cameras.empty())
        {
            Debug::Warning("No main camera present on the scene. Rendering will not be performed!");
            return;
        }

        auto cameraGO = *cameras.begin();
        m_CurrentRenderInfo.MainCamera = cameraGO->GetComponent<Camera>();
        m_CurrentRenderInfo.Models = scene->FindComponentsOfType<ModelRenderer>();
    }

    void Renderer::Render() const
    {
        m_BackendRenderer->Render();
    }

    void Renderer::WaitForFinish() const
    {
        m_BackendRenderer->WaitForFinish();
    }

    GraphicsApi Renderer::SelectGraphicsAPI()
    {
        // For now only vulkan is supported
        return GraphicsApi::Vulkan;
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
}
