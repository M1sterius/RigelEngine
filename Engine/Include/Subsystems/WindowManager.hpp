#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "EngineEvents.hpp"

#include "Math.hpp"

#include <string>
#include <vector>

struct GLFWwindow;

namespace rge
{
    struct MonitorVideoModInfo
    {
        MonitorVideoModInfo() = default;

        MonitorVideoModInfo(const glm::uvec2 resolution, const uint32_t refreshRate, const uint32_t bitsPerColor)
            :   Resolution(resolution), RefreshRate(refreshRate), BitsPerColor(bitsPerColor) { }

        NODISCARD inline std::string ToString() const
        {
            return std::to_string(Resolution.x) + "x" + std::to_string(Resolution.y) + " @" + std::to_string(RefreshRate) + "Hz";
        }

        glm::uvec2 Resolution = {0, 0};
        uint32_t RefreshRate = 0;
        uint32_t BitsPerColor = 0;
    };

    struct MonitorInfo
    {
        MonitorInfo() = default;

        uint32_t ID = 0;
        std::string Name;
        MonitorVideoModInfo CurrentVideoMod {};
        std::vector<MonitorVideoModInfo> VideoMods {};
    };

    class EventManager;

    class WindowManager final : public RigelSubsystem
    {
    public:
        NODISCARD inline glm::uvec2 GetSize() const { return m_WindowSize; }
    INTERNAL:
        WindowManager();
        ~WindowManager();

        NODISCARD inline GLFWwindow* GetGLFWWindowPtr() const { return m_GLFWWindow; }

        NODISCARD bool WindowShouldClose() const;
    private:
        void Startup() override;
        void Shutdown() override;

        void OnWindowResize(const WindowResizeEvent& event);

        void EnumerateMonitorInfo();

        GLFWwindow* m_GLFWWindow = nullptr;
        glm::uvec2 m_WindowSize = {1280, 720}; // Default window size, should be changed based on engine config file.
        std::string m_WindowTitle = "Rigel engine app.";
        bool m_WindowResizeFlag = false;

        std::vector<MonitorInfo> m_Monitors;
    };
}
