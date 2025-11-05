#pragma once

#include "Core.hpp"
#include "SceneRenderInfo.hpp"
#include "Subsystems/RigelSubsystem.hpp"

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
        ErrorCode LateStartup() const;

        void Render();

        NODISCARD Backend::Vulkan::VK_Renderer& GetImpl() const { return *m_Impl; }
        NODISCARD Backend::Vulkan::VK_ImGUI_Renderer& GetImGuiImpl() const { return *m_ImGuiImpl; }

        NODISCARD Ref<Backend::SceneRenderInfo> GetSceneRenderInfo() { return &m_SceneRenderInfo; }

        // Blocks the calling thread until all rendering operations are done
        void WaitForFinish() const;
    private:
        Backend::SceneRenderInfo m_SceneRenderInfo;

        std::unique_ptr<Backend::Vulkan::VK_Renderer> m_Impl;
        std::unique_ptr<Backend::Vulkan::VK_ImGUI_Renderer> m_ImGuiImpl;
    };
}
