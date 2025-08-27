#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "DisplayMode.hpp"
#include "CursorState.hpp"
#include "Subsystems/RigelSubsystem.hpp"

#include <string>
#include <vector>

struct GLFWwindow;
struct GLFWmonitor;

namespace Rigel
{
    class ProjectSettings;

    class WindowManager final : public RigelSubsystem
    {
    private:
        struct MonitorVideoModInfo
        {
            glm::uvec2 Resolution = {};
            uint32_t RefreshRate = 0;
            glm::uvec3 BitsPerColor = {};
        };

        struct MonitorInfo
        {
            const char* Name = "";
            bool Primary = false;
            GLFWmonitor* GLFWmonitorPtr = nullptr;
            MonitorVideoModInfo CurrentMod = {};
            std::vector<MonitorVideoModInfo> AvailableModes = {};
        };
    public:
        NODISCARD glm::uvec2 GetWindowSize() const { return m_WindowSize; }
        void SetVsyncState(const bool state);

        void SetDisplayMode(const DisplayMode mode);
        NODISCARD DisplayMode GetDisplayMode() const { return m_CurrentDisplayMode; }

        void SetCursorState(const CursorState state);
        NODISCARD CursorState GetCursorState() const { return m_CurrentCursorState; }
    INTERNAL:
        WindowManager() = default;
        ~WindowManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        void PollGLFWEvents() const;
        void WaitForFocus() const;

        NODISCARD GLFWwindow* GetGLFWWindowPtr() const { return m_GLFWWindow; }
        NODISCARD bool WindowShouldClose() const;

        NODISCARD bool IsVsyncEnabled() const { return m_VsyncEnabled; }

        NODISCARD bool GetWindowResizeFlag() const { return m_WindowResizeFlag; }void ResetWindowResizeFlag() { m_WindowResizeFlag = false; }
    private:
        NODISCARD ErrorCode EnumerateMonitorInfo();

        glm::uvec2 m_WindowSize = {1280, 720};
        glm::ivec2 m_WindowPosition = {};
        bool m_VsyncEnabled = false;
        std::string m_WindowTitle = "Rigel engine game.";
        bool m_WindowResizeFlag = false;

        GLFWwindow* m_GLFWWindow = nullptr;
        uint32_t m_PrimaryMonitorIndex = -1;
        std::vector<MonitorInfo> m_Monitors;
        DisplayMode m_CurrentDisplayMode = DisplayMode::Windowed;
        CursorState m_CurrentCursorState = CursorState::Normal;

        friend void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
        friend void window_move_callback(GLFWwindow* window, int xPos, int yPos);
    };
}
