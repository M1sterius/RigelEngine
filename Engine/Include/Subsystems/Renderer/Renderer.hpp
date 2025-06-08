#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "SceneRenderInfo.hpp"

#include <memory>

namespace Rigel
{
    class ProjectSettings;

    namespace Backend::Vulkan
    {
        class VK_Renderer;
        class VK_ImGUI_Renderer;
    }

    class Renderer final : public RigelSubsystem
    {
    public:
    INTERNAL:
        Renderer();
        ~Renderer() override;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        NODISCARD Backend::Vulkan::VK_Renderer& GetBackend() const;
        NODISCARD Backend::Vulkan::VK_ImGUI_Renderer& GetImGuiBackend() const { return *m_ImGuiBackend; }

        NODISCARD Backend::SceneRenderInfo& GetSceneRenderInfo() { return m_CurrentRenderInfo; }

        ErrorCode LateStartup() const;
        void Prepare();
        void Render() const;

        // Blocks the calling thread until all rendering operation are done
        void WaitForFinish() const;
    private:
        Backend::SceneRenderInfo m_CurrentRenderInfo;

        std::unique_ptr<Backend::Vulkan::VK_Renderer> m_BackendRenderer;
        std::unique_ptr<Backend::Vulkan::VK_ImGUI_Renderer> m_ImGuiBackend;
    };
}
