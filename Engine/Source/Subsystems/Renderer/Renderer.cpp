#include "Renderer.hpp"
#include "Camera.hpp"
#include "ComponentHandle.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "ModelRenderer.hpp"
#include "SubsystemGetters.hpp"
#include "Vulkan/VK_Renderer.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"

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

        m_CurrentRenderInfo.Reset();

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

        auto scene = GetSceneManager()->GetLoadedScene();
        if (scene.IsNull())
            return;

        auto cameras = scene->FindComponentsOfType<Camera>();
        if (cameras.empty())
        {
            Debug::Warning("No main camera present on the scene. Rendering will not be performed!");
            return;
        }

        m_CurrentRenderInfo.CameraPresent = true;
        m_CurrentRenderInfo.ProjView = cameras[0]->GetProjection() * cameras[0]->GetView();

        for (const auto& mr : scene->FindComponentsOfType<ModelRenderer>())
        {
            if (const auto asset = mr->GetModelAsset(); !asset.IsNull() && asset->IsOK())
            {
                m_CurrentRenderInfo.Models.push_back(asset);
                m_CurrentRenderInfo.Transforms.push_back(mr->GetGameObject()->GetTransform()->GetWorldMatrix());
            }
        }
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
