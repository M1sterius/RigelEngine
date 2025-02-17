#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include "Math.hpp"

struct GLFWwindow;

namespace rge
{
    class WindowManager final : public RigelSubsystem
    {
    INTERNAL:
        WindowManager();
        ~WindowManager();
    private:
        void Startup() override;
        void Shutdown() override;

        GLFWwindow* m_GLFWWindow = nullptr;
        glm::uvec2 m_WindowSize = {1280, 720}; // Default window size, should be changed based on engine config file.
        std::string m_WindowTitle = "Rigel engine app.";
    };
}