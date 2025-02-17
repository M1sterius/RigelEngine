#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "EngineEvents.hpp"

#include "Math.hpp"

struct GLFWwindow;

namespace rge
{
    class EventManager;

    class WindowManager final : public RigelSubsystem
    {
    INTERNAL:
        WindowManager();
        ~WindowManager();

        NODISCARD bool WindowShouldClose() const;
    private:
        void Startup() override;
        void Shutdown() override;

        void OnPollGlfwEvents(const Event& e) const;
        void OnWindowResize(const Event& e);

        GLFWwindow* m_GLFWWindow = nullptr;
        glm::uvec2 m_WindowSize = {1280, 720}; // Default window size, should be changed based on engine config file.
        std::string m_WindowTitle = "Rigel engine app.";

        bool m_WindowResizeFlag = false;
    };
}
