#include "WindowManager.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "InternalEvents.hpp"
#include "Debug.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

#include <format>

namespace Rigel
{
#pragma region GLFW_Callbacks
    void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
    {
        auto& windowManager = Engine::Get().GetWindowManager();

        const auto newSize = glm::uvec2(width, height);
        windowManager.m_WindowResizeFlag = true;
        windowManager.m_WindowSize = newSize;

        Engine::Get().GetEventManager().Dispatch<WindowResizeEvent>(WindowResizeEvent(newSize));
    }

    void window_move_callback(GLFWwindow* window, int xPos, int yPos)
    {
        auto& windowManager = Engine::Get().GetWindowManager();
        windowManager.m_WindowPosition = glm::vec2(xPos, yPos);
    }
#pragma endregion

    WindowManager::WindowManager() { Startup(); }
    WindowManager::~WindowManager() { Shutdown(); }

    bool WindowManager::WindowShouldClose() const { return glfwWindowShouldClose(m_GLFWWindow); }

    void WindowManager::Startup()
    {
        Debug::Trace("Staring up window manager.");

        if (!glfwInit())
            throw RigelException("Window manager initialization failed! glfw initialization failed.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable opengl api for vulkan.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        EnumerateMonitorInfo();
        m_GLFWWindow = glfwCreateWindow(static_cast<int>(m_WindowSize.x), static_cast<int>(m_WindowSize.y),
            m_WindowTitle.c_str(), nullptr, nullptr);

        if (!m_GLFWWindow)
            throw RigelException("Window manager initialization failed. Failed to create glfw window.");

        int winPosX, winPosY;
        glfwGetWindowPos(m_GLFWWindow, &winPosX, &winPosY);
        m_WindowPosition = glm::ivec2(winPosX, winPosY);

        Debug::Trace("GLFW window instance successfully created.");

        auto& eventManager = Engine::Get().GetEventManager();

        glfwSetWindowUserPointer(m_GLFWWindow, this);

        glfwSetFramebufferSizeCallback(m_GLFWWindow, framebuffer_resize_callback);
        glfwSetWindowPosCallback(m_GLFWWindow, window_move_callback);

        m_Initialized = true;
    }

    void WindowManager::Shutdown()
    {
        Debug::Trace("Shutting down window manager.");

        glfwDestroyWindow(m_GLFWWindow);
        glfwTerminate();
    }

    void WindowManager::SetScreenMode(const ScreenMode mode)
    {
        if (mode == m_CurrentScreenMode) return;
        m_CurrentScreenMode = mode;

        if (mode == ScreenMode::Fullscreen)
        {
            const auto& primaryMonitor = m_Monitors[m_PrimaryMonitorIndex];

            glfwSetWindowMonitor(m_GLFWWindow, primaryMonitor.GLFWmonitorPtr,
                 0, 0, primaryMonitor.CurrentMod.Resolution.x,
                 primaryMonitor.CurrentMod.Resolution.y, primaryMonitor.CurrentMod.RefreshRate);
        }
        else if (mode == ScreenMode::Windowed)
        {
            // Use arbitrary window size and position values because the correct ones always
            // get overridden when switching to fullscreen
            glfwSetWindowMonitor(m_GLFWWindow, nullptr, 100, 100, 1280, 720, 0);
        }
    }

    void WindowManager::EnumerateMonitorInfo()
    {
        int monitorCount;
        const auto monitors = glfwGetMonitors(&monitorCount);

        if (monitorCount == 0)
            throw RigelException("Failed to detect any connected monitors!");

        m_Monitors.resize(monitorCount);

        for (size_t i = 0; i < m_Monitors.size(); i++)
        {
            auto& monitorInfo = m_Monitors[i];
            const auto glfwMonitor = monitors[i];

            monitorInfo.GLFWmonitorPtr = glfwMonitor;
            monitorInfo.Name = glfwGetMonitorName(glfwMonitor);
            monitorInfo.Primary = (monitorInfo.GLFWmonitorPtr == glfwGetPrimaryMonitor());

            if (monitorInfo.Primary) m_PrimaryMonitorIndex = i;

            const auto currentGlfwVideoMod = glfwGetVideoMode(glfwMonitor);
            monitorInfo.CurrentMod = {glm::uvec2(currentGlfwVideoMod->width, currentGlfwVideoMod->height),
                      static_cast<uint32_t>(currentGlfwVideoMod->refreshRate),
                      glm::uvec3(currentGlfwVideoMod->redBits, currentGlfwVideoMod->greenBits, currentGlfwVideoMod->blueBits)};

            int modeCount;
            const auto modes = glfwGetVideoModes(glfwMonitor, &modeCount);
            monitorInfo.AvailableModes.resize(modeCount);

            for (size_t j = 0; j < modeCount; j++)
            {
                // Retrieve info about all video modes a monitor can support

                const auto glfwMode = modes[j];
                auto& mode = monitorInfo.AvailableModes[j];

                mode.Resolution = glm::uvec2(glfwMode.width, glfwMode.height);
                mode.RefreshRate = glfwMode.refreshRate;
                mode.BitsPerColor = glm::uvec3(glfwMode.redBits, glfwMode.greenBits, glfwMode.blueBits);
            }
        }

        if (m_PrimaryMonitorIndex == -1)
            throw RigelException("Failed to detect primary monitor!");

        Debug::Trace(std::format("Detected {} available monitors:", m_Monitors.size()));
        for (const auto& monitor : m_Monitors)
        {
            auto text = std::format("    {}. {}x{} @{}Hz ", monitor.Name, monitor.CurrentMod.Resolution.x,
                            monitor.CurrentMod.Resolution.y, monitor.CurrentMod.RefreshRate);
            if (monitor.Primary) text += "[PRIMARY]";
            Debug::Trace(text);
        }
    }

    void WindowManager::PollGLFWEvents() const
    {
        glfwPollEvents();
    }

    void WindowManager::WaitForFocus() const
    {
        auto size = GetSize();
        while (size.x == 0 || size.y == 0)
        {
            size = GetSize();
            glfwWaitEvents();
        }
    }
}
