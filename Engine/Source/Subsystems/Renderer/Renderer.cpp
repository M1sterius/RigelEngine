#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/Renderer/RenderScene.hpp"
#include "Subsystems/SceneManager.hpp"
#include "Subsystems/EventSystem/EventManager.hpp"
#include "Subsystems/EventSystem/EngineEvents.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Backend/Renderer/Vulkan/VK_Renderer.hpp"
#include "Backend/Renderer/Vulkan/ImGui/VK_ImGUI_Renderer.hpp"

#include "imgui/imgui.h"

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

    void Renderer::Render()
    {
        m_ImGuiImpl->BeginNewFrame();
        GetEventManager()->Dispatch(DrawGUIEvent());
        ImGui::Render();

        // It's more optimal to cache RenderScene and just update it instead of creating a new instance every frame
        const auto renderScene = RenderScene::Extract(GetSceneManager()->GetLoadedScene());
        m_Impl->Render(renderScene);
    }

    void Renderer::WaitForFinish() const
    {
        m_Impl->WaitForFinish();
    }
}
