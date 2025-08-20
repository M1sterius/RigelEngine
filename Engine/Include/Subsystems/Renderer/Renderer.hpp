#pragma once

#include "Core.hpp"
#include "Subsystems/RigelSubsystem.hpp"
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

        NODISCARD inline Backend::Vulkan::VK_Renderer& GetImpl() const { return *m_Impl; }
        NODISCARD inline Backend::Vulkan::VK_ImGUI_Renderer& GetImGuiImpl() const { return *m_ImGuiImpl; }

        NODISCARD Backend::SceneRenderInfo& GetSceneRenderInfo() { return m_CurrentRenderInfo; }

        ErrorCode LateStartup() const;

        void Render();

        // Blocks the calling thread until all rendering operations are done
        void WaitForFinish() const;
    private:
        Backend::SceneRenderInfo m_CurrentRenderInfo;

        std::unique_ptr<Backend::Vulkan::VK_Renderer> m_Impl;
        std::unique_ptr<Backend::Vulkan::VK_ImGUI_Renderer> m_ImGuiImpl;
    };
}
