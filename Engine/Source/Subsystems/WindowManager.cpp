#include "WindowManager.hpp"

#include "Engine.hpp"
#include "EventManager.hpp"
#include "Debug.hpp"
#include "Logger.hpp"

#include "glfw3.h"

namespace rge
{

#pragma region GLFW_Callbacks
    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
    {
        const auto newSize = glm::uvec2(width, height);
        Engine::Get().GetEventManager().Dispatch<WindowResizeEvent>(WindowResizeEvent(newSize));
    }

    static void key_action_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {

    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {

    }

    static void mouse_move_callback(GLFWwindow* window, double xPos, double yPos)
    {

    }

    static void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
    {

    }
#pragma endregion

    WindowManager::WindowManager() { Startup(); }
    WindowManager::~WindowManager() { Shutdown(); }

    bool WindowManager::WindowShouldClose() const { return glfwWindowShouldClose(m_GLFWWindow); }

    void WindowManager::Startup()
    {
        VERBOSE_MESSAGE("Staring up window manager.");

        if (!glfwInit())
        {
            THROW_RUNTIME_ERROR("Window manager initialization failed! glfw initialization failed.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable opengl api for vulkan.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_GLFWWindow = glfwCreateWindow(static_cast<int>(m_WindowSize.x), static_cast<int>(m_WindowSize.y),
            m_WindowTitle.c_str(), nullptr, nullptr);

        if (!m_GLFWWindow)
        {
            THROW_RUNTIME_ERROR("Window manager initialization failed. Failed to create glfw window.");
        }

        VERBOSE_MESSAGE("GLFW window instance successfully created.");

        auto& eventManager = Engine::Get().GetEventManager();

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, framebuffer_resize_callback);

        eventManager.Subscribe<PollEventsEvent>(
        [this](const Event& e) { this->OnPollGlfwEvents(e); }
        );

        eventManager.Subscribe<WindowResizeEvent>(
        [this](const Event& e) { this->OnWindowResize(e); }
        );
    }

    void WindowManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down window manager.");
    }

    void WindowManager::OnPollGlfwEvents(const Event& e) const
    {
        const auto& event = CastEvent<PollEventsEvent>(e);
        glfwPollEvents();
    }

    void WindowManager::OnWindowResize(const Event& e)
    {
        const auto& event = CastEvent<WindowResizeEvent>(e);
        m_WindowResizeFlag = true;
        m_WindowSize = event.NewSize;
    }

    void WindowManager::EnumerateMonitorInfo()
    {
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);

        for (size_t i = 0; i < count; i++)
        {
            GLFWmonitor* monitor = monitors[i];
            const char* name = glfwGetMonitorName(monitor);
            printf("Monitor %d: %s\n", i + 1, name);

            int modeCount;
            const GLFWvidmode* modes = glfwGetVideoModes(monitor, &modeCount);

            printf("Supported video modes:\n");
            for (int j = 0; j < modeCount; j++)
            {
                printf("  %dx%d @ %dHz (RGB Bits: %d, %d, %d)\n",
                       modes[j].width, modes[j].height, modes[j].refreshRate,
                       modes[j].redBits, modes[j].greenBits, modes[j].blueBits);
            }

            const GLFWvidmode* currentMode = glfwGetVideoMode(monitor);
            printf("Current video mode: %dx%d @ %dHz\n",
                   currentMode->width, currentMode->height, currentMode->refreshRate);
        }
    }
}
