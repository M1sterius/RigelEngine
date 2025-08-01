#include "InputManager.hpp"
#include "EngineEvents.hpp"
#include "EventManager.hpp"
#include "WindowManager.hpp"
#include "SubsystemGetters.hpp"
#include "Engine.hpp"
#include "Debug.hpp"

#include "glfw3.h"

namespace Rigel
{
#pragma region GLFW_Callbacks
    void key_action_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto inputManager = GetInputManager();
        auto eventManager = GetEventManager();

        if (action == GLFW_PRESS)
        {
            eventManager->Dispatch<KeyDownEvent>(KeyDownEvent(static_cast<KeyCode>(key)));
            inputManager->m_KeyboardKeys.insert(static_cast<KeyCode>(key));
        }
        else if (action == GLFW_REPEAT)
        {
            eventManager->Dispatch<KeyPressedEvent>(KeyPressedEvent(static_cast<KeyCode>(key)));
        }
        else if (action == GLFW_RELEASE)
        {
            eventManager->Dispatch<KeyUpEvent>(KeyUpEvent(static_cast<KeyCode>(key)));
            inputManager->m_KeyboardKeys.erase(static_cast<KeyCode>(key));
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto inputManager = GetInputManager();
        auto eventManager = GetEventManager();

        if (action == GLFW_PRESS)
        {
            eventManager->Dispatch<MouseButtonDownEvent>(MouseButtonDownEvent(static_cast<MouseButton>(button)));
            inputManager->m_MouseButtons.insert(static_cast<MouseButton>(button));
        }
        else if (action == GLFW_REPEAT)
        {
            eventManager->Dispatch<MouseButtonPressedEvent>(MouseButtonPressedEvent(static_cast<MouseButton>(button)));
        }
        else if (action == GLFW_RELEASE)
        {
            eventManager->Dispatch<MouseButtonUpEvent>(MouseButtonUpEvent(static_cast<MouseButton>(button)));
            inputManager->m_MouseButtons.erase(static_cast<MouseButton>(button));
        }
    }

    void mouse_move_callback(GLFWwindow* window, double xPos, double yPos)
    {
        auto inputManager = GetInputManager();
        auto eventManager = GetEventManager();

        const auto pos = glm::vec2(xPos, yPos);
        const auto mouseDelta = pos - inputManager->m_MousePosition;
        inputManager->m_MousePosition = pos;

        eventManager->Dispatch<MouseMoveEvent>(MouseMoveEvent(inputManager->m_MousePosition, mouseDelta));
    }

    void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
    {
        GetEventManager()->Dispatch<MouseScrollEvent>(MouseScrollEvent(glm::vec2(xOffset, yOffset)));
    }
#pragma endregion

    ErrorCode InputManager::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up input manager.");

        m_GLFWWindow = GetWindowManager()->GetGLFWWindowPtr();
        if (!m_GLFWWindow)
        {
            Debug::Error("InputManager::Failed to retrieve GLFWWindow instance!");
            return ErrorCode::UNKNOWN;
        }

        m_KeyboardKeys = std::unordered_set<KeyCode>();
        m_OldKeyboardKeys = std::unordered_set<KeyCode>();

        m_MouseButtons = std::unordered_set<MouseButton>();
        m_OldMouseButtons = std::unordered_set<MouseButton>();

        glfwSetKeyCallback(m_GLFWWindow, key_action_callback);
        glfwSetMouseButtonCallback(m_GLFWWindow, mouse_button_callback);
        glfwSetCursorPosCallback(m_GLFWWindow, mouse_move_callback);
        glfwSetScrollCallback(m_GLFWWindow, mouse_scroll_callback);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    ErrorCode InputManager::Shutdown()
    {
        Debug::Trace("Shutting down input manager");
        return ErrorCode::OK;
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

    bool InputManager::GetKeyDown(const KeyCode key) const
    {
        return InKeys(key) && !InOldKeys(key);
    }

    bool InputManager::GetKeyPressed(const KeyCode key) const
    {
        return InKeys(key);
    }

    bool InputManager::GetKeyUp(const KeyCode key) const
    {
        return !InKeys(key) && InOldKeys(key);
    }

    bool InputManager::GetMouseButtonDown(const MouseButton button) const
    {
        return InMouseButtons(button) && !InOldMouseButtons(button);
    }

    bool InputManager::GetMouseButtonPressed(const MouseButton button) const
    {
        return InMouseButtons(button);
    }

    bool InputManager::GetMouseButtonUp(const MouseButton button) const
    {
        return !InMouseButtons(button) && InOldMouseButtons(button);
    }

    glm::vec2 InputManager::GetMousePosition() const
    {
        return m_MousePosition;
    }

    glm::vec2 InputManager::GetMouseDelta() const
    {
        return m_MouseDelta;
    }
}
