#include "Input.hpp"
#include "InputManager.hpp"
#include "Engine.hpp"

namespace Rigel
{
    bool Input::GetKeyDown(const KeyCode key)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.InKeys(key) && !inputManager.InOldKeys(key);
    }

    bool Input::GetKeyPressed(const KeyCode key)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.InKeys(key);
    }

    bool Input::GetKeyUp(const KeyCode key)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return !inputManager.InKeys(key) && inputManager.InOldKeys(key);
    }

    glm::vec2 Input::GetMousePosition()
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.m_MousePosition;
    }

    glm::vec2 Input::GetMouseDelta()
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.m_MouseDelta;
    }

    bool Input::GetMouseButtonDown(const MouseButton button)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.InMouseButtons(button) && !inputManager.InOldMouseButtons(button);
    }

    bool Input::GetMouseButtonPressed(const MouseButton button)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return inputManager.InMouseButtons(button);
    }

    bool Input::GetMouseButtonUp(const MouseButton button)
    {
        const auto& inputManager = Engine::Get().GetInputManager();
        return !inputManager.InMouseButtons(button) && inputManager.InOldMouseButtons(button);
    }
}
