#include "InputManager.hpp"
#include "InternalEvents.hpp"
#include "EventManager.hpp"
#include "WindowManager.hpp"
#include "Engine.hpp"
#include "Debug.hpp"

#include "glfw3.h"

namespace rge
{
#pragma region GLFW_Callbacks
    void key_action_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        const auto& engine = Engine::Get();
        auto& inputManager = engine.GetInputManager();
        auto& eventManager = engine.GetEventManager();

        if (action == GLFW_PRESS)
        {
            eventManager.Dispatch<KeyDownEvent>(KeyDownEvent(static_cast<KeyCode>(key)));
            inputManager.m_KeyboardKeys.insert(static_cast<KeyCode>(key));
        }
        else if (action == GLFW_REPEAT)
        {
            eventManager.Dispatch<KeyPressedEvent>(KeyPressedEvent(static_cast<KeyCode>(key)));
        }
        else if (action == GLFW_RELEASE)
        {
            eventManager.Dispatch<KeyUpEvent>(KeyUpEvent(static_cast<KeyCode>(key)));
            inputManager.m_KeyboardKeys.erase(static_cast<KeyCode>(key));
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        const auto& engine = Engine::Get();
        auto& inputManager = engine.GetInputManager();
        auto& eventManager = engine.GetEventManager();

        if (action == GLFW_PRESS)
        {
            eventManager.Dispatch<MouseButtonDownEvent>(MouseButtonDownEvent(static_cast<MouseButton>(button)));
            inputManager.m_MouseButtons.insert(static_cast<MouseButton>(button));
        }
        else if (action == GLFW_REPEAT)
        {
            eventManager.Dispatch<MouseButtonPressedEvent>(MouseButtonPressedEvent(static_cast<MouseButton>(button)));
        }
        else if (action == GLFW_RELEASE)
        {
            eventManager.Dispatch<MouseButtonUpEvent>(MouseButtonUpEvent(static_cast<MouseButton>(button)));
            inputManager.m_MouseButtons.erase(static_cast<MouseButton>(button));
        }
    }

    void mouse_move_callback(GLFWwindow* window, double xPos, double yPos)
    {
        const auto& engine = Engine::Get();
        auto& inputManager = engine.GetInputManager();
        auto& eventManager = engine.GetEventManager();

        const auto pos = glm::vec2(xPos, yPos);
        const auto mouseDelta = pos - inputManager.m_MousePosition;
        inputManager.m_MousePosition = pos;

        eventManager.Dispatch<MouseMoveEvent>(MouseMoveEvent(inputManager.m_MousePosition, mouseDelta));
    }

    void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
    {
        const auto& engine = Engine::Get();
        auto& inputManager = engine.GetInputManager();
        auto& eventManager = engine.GetEventManager();

        eventManager.Dispatch<MouseScrollEvent>(MouseScrollEvent(glm::vec2(xOffset, yOffset)));
    }
#pragma endregion

    InputManager::InputManager() { Startup(); }
    InputManager::~InputManager() { Shutdown(); }

    void InputManager::Startup()
    {
        Debug::Trace("Starting up input manager.");

        const auto& engine = Engine::Get();

        m_GLFWWindow = engine.GetWindowManager().GetGLFWWindowPtr();
        if (!m_GLFWWindow)
            throw RigelException("Input manager failed to retrieve GLFWWindow instance. Input manager initialization failed!");

        m_KeyboardKeys = std::unordered_set<KeyCode>();
        m_OldKeyboardKeys = std::unordered_set<KeyCode>();

        m_MouseButtons = std::unordered_set<MouseButton>();
        m_OldMouseButtons = std::unordered_set<MouseButton>();

        glfwSetKeyCallback(m_GLFWWindow, key_action_callback);
        glfwSetMouseButtonCallback(m_GLFWWindow, mouse_button_callback);
        glfwSetCursorPosCallback(m_GLFWWindow, mouse_move_callback);
        glfwSetScrollCallback(m_GLFWWindow, mouse_scroll_callback);

        m_Initialized = true;
    }

    void InputManager::ResetInputState()
    {
        m_OldKeyboardKeys.clear();
        m_OldKeyboardKeys.insert(m_KeyboardKeys.begin(), m_KeyboardKeys.end());

        m_OldMouseButtons.clear();
        m_OldMouseButtons.insert(m_MouseButtons.begin(), m_MouseButtons.end());

        m_MouseDelta = m_MousePosition - m_OldMousePosition;
        m_OldMousePosition = m_MousePosition;
    }

    void InputManager::Shutdown()
    {
        Debug::Trace("Shutting down input manager");
    }
}
