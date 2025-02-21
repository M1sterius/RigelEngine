#include "InputManager.hpp"

#include <WindowManager.hpp>

#include "Engine.hpp"
#include "Logger.hpp"

namespace rge
{
    InputManager::InputManager() { Startup(); }

    InputManager::~InputManager() { Shutdown(); }

    void InputManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up input manager.");

        m_GLFWwindow = Engine::Get().GetWindowManager().GetGLFWWindowPtr();
        if (!m_GLFWwindow)
        {
            THROW_RUNTIME_ERROR("Input manager failed to retrieve GLFWwindow instance. Input manager initialization failed!");
        }
    }

    void InputManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down input manager");
    }
}
