#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "ScreenMode.hpp"

#include "Math.hpp"

#include <string>
#include <vector>

struct GLFWwindow;
struct GLFWmonitor;

namespace Rigel
{
    class ProjectSettings;

    struct MonitorVideoModInfo
    {
        glm::uvec2 Resolution = {0, 0};
        uint32_t RefreshRate = 0;
        glm::uvec3 BitsPerColor = { 0, 0, 0};
    };

    struct MonitorInfo
    {
        const char* Name = "";
        bool Primary = false;
        GLFWmonitor* GLFWmonitorPtr = nullptr;
        MonitorVideoModInfo CurrentMod = {};
        std::vector<MonitorVideoModInfo> AvailableModes = {};
    };

    class WindowManager final : public RigelSubsystem
    {
    public:
        NODISCARD inline glm::uvec2 GetSize() const { return m_WindowSize; }

        void SetScreenMode(const ScreenMode mode);
        NODISCARD inline ScreenMode GetScreenMode() const { return m_CurrentScreenMode; }
    INTERNAL:
        WindowManager() = default;
        ~WindowManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        void PollGLFWEvents() const;
        void WaitForFocus() const;

        NODISCARD inline GLFWwindow* GetGLFWWindowPtr() const { return m_GLFWWindow; }
        NODISCARD bool WindowShouldClose() const;
        NODISCARD bool IsVsyncEnabled() const { return m_VsyncEnabled; }

        NODISCARD inline bool GetWindowResizeFlag() const { return m_WindowResizeFlag; }
        inline void ResetWindowResizeFlag() { m_WindowResizeFlag = false; }
    private:
        NODISCARD int32_t EnumerateMonitorInfo();

        glm::uvec2 m_WindowSize = {1280, 720}; // Default window size should be changed based on an engine config file.
        glm::ivec2 m_WindowPosition = {};
        bool m_VsyncEnabled = false;
        std::string m_WindowTitle = "Rigel engine app.";
        bool m_WindowResizeFlag = false;

        GLFWwindow* m_GLFWWindow = nullptr;
        uint32_t m_PrimaryMonitorIndex = -1;
        std::vector<MonitorInfo> m_Monitors;
        ScreenMode m_CurrentScreenMode = ScreenMode::Windowed;

        friend void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
        friend void window_move_callback(GLFWwindow* window, int xPos, int yPos);
    };
}
