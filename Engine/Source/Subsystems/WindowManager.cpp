#include "WindowManager.hpp"
#include "Logger.hpp"

#include "glfw3.h"

namespace rge
{
    WindowManager::WindowManager() { Startup(); }

    WindowManager::~WindowManager() { Shutdown(); }

    void WindowManager::Startup()
    {
        VERBOSE_MESSAGE("Staring up window manager.");

        if (!glfwInit())
        {
            THROW_RUNTIME_ERROR("Window manager initialization failed! glfw initialization failed.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable opengl api for vulkan.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_GLFWWindow = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, m_WindowTitle.c_str(), nullptr, nullptr);
        if (!m_GLFWWindow)
        {
            THROW_RUNTIME_ERROR("Window manager initialization failed. Failed to create glfw window.");
        }
    }

    void WindowManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down window manager.");
    }
}
