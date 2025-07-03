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

        m_Impl = std::make_unique<Backend::Vulkan::VK_Renderer>();
        m_ImGuiImpl = std::make_unique<Backend::Vulkan::VK_ImGUI_Renderer>(*m_Impl);

        if (const auto result = m_Impl->Startup(); result != ErrorCode::OK)
        {
            Debug::Error("Failed to start up vulkan rendering backend! Error code: {}.", static_cast<int32_t>(result));
            return ErrorCode::RENDERER_BACKEND_START_UP_FAILURE;
        }

        if (const auto result = m_ImGuiImpl->Startup(); result != ErrorCode::OK)
        {
            Debug::Error("Failed to start up imgui backend for vulkan! Error code: {}.", static_cast<int32_t>(result));
            return ErrorCode::IMGUI_BACKEND_STARTUP_FAILURE;
        }

        m_Impl->SetImGuiBackend(m_ImGuiImpl.get());

        m_Initialized = true;
        return ErrorCode::OK;
    }

    ErrorCode Renderer::Shutdown()
    {
        Debug::Trace("Shutting down renderer.");

        m_ImGuiImpl->Shutdown();
        m_Impl->Shutdown();

        m_ImGuiImpl.reset();
        m_Impl.reset();

        return ErrorCode::OK;
    }

    ErrorCode Renderer::LateStartup() const
    {
        return m_Impl->LateStartup();
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
        // m_CurrentRenderInfo.Models = {};
    }

    void Renderer::Render() const
    {
        m_Impl->Render();
    }

    void Renderer::WaitForFinish() const
    {
        m_Impl->WaitForFinish();
    }
}
