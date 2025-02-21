#include "InputManager.hpp"
#include "InternalEvents.hpp"
#include "WindowManager.hpp"
#include "EventManager.hpp"
#include "Engine.hpp"
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

    InputManager::InputManager() { Startup(); }
    InputManager::~InputManager() { Shutdown(); }

    void InputManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up input manager.");

        const auto& engine = Engine::Get();

        m_GLFWWindow = engine.GetWindowManager().GetGLFWWindowPtr();
        if (!m_GLFWWindow)
        {
            THROW_RUNTIME_ERROR("Input manager failed to retrieve GLFWwindow instance. Input manager initialization failed!");
        }

        engine.GetEventManager().Subscribe<InputUpdateEvent>(
            [this](const InputUpdateEvent&) { this->OnInputUpdate(); }
        );

        glfwSetFramebufferSizeCallback(m_GLFWWindow, framebuffer_resize_callback);
        glfwSetKeyCallback(m_GLFWWindow, key_action_callback);
        glfwSetMouseButtonCallback(m_GLFWWindow, mouse_button_callback);
        glfwSetCursorPosCallback(m_GLFWWindow, mouse_move_callback);
        glfwSetScrollCallback(m_GLFWWindow, mouse_scroll_callback);
    }

    void InputManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down input manager");
    }

    void InputManager::OnInputUpdate()
    {

    }
}
