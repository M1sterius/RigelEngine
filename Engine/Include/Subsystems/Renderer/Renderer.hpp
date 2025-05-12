#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "SceneRenderInfo.hpp"

#include <memory>

namespace Rigel
{
    class ProjectSettings;

    namespace Backend
    {
        class IRendererBackend;
        class IImGuiBackend;
    }

    enum class GraphicsApi : uint8_t
    {
        None,
        Vulkan,
        OpenGL
    };

    class Renderer final : public RigelSubsystem
    {
    public:
        NODISCARD GraphicsApi GetSelectedGraphicsAPI() const { return m_SelectedGraphicsAPI; }
        NODISCARD static std::string GetGraphicsAPIString(const GraphicsApi api);
    INTERNAL:
        Renderer();
        ~Renderer() override;

        int32_t Startup(const ProjectSettings& settings) override;
        int32_t Shutdown() override;

        NODISCARD Backend::IRendererBackend& GetBackend() const;
        NODISCARD Backend::IImGuiBackend& GetImGuiBackend() const { return *m_ImGuiBackend; }

        NODISCARD Backend::SceneRenderInfo& GetSceneRenderInfo() { return m_CurrentRenderInfo; }

        void LateInit() const;
        void Prepare();
        void Render() const;

        // Blocks the calling thread until all rendering operation are done
        void WaitForFinish() const;
    private:
        Backend::SceneRenderInfo m_CurrentRenderInfo;

        NODISCARD static GraphicsApi SelectGraphicsAPI();
        GraphicsApi m_SelectedGraphicsAPI = GraphicsApi::None;

        std::unique_ptr<Backend::IRendererBackend> m_BackendRenderer;
        std::unique_ptr<Backend::IImGuiBackend> m_ImGuiBackend;
    };
}
