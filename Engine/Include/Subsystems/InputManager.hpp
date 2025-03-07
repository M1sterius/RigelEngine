#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "KeysAndButtons.hpp"

#include <unordered_set>

struct GLFWwindow;

namespace rge
{
    class InputManager final : public RigelSubsystem
    {
    INTERNAL:
        InputManager();
        ~InputManager() override;

        void InputUpdate();
    private:
        void Startup() override;
        void Shutdown() override;

        NODISCARD inline bool InKeys(const KeyCode key) const { return m_KeyboardKeys.find(key) != m_KeyboardKeys.end(); }
        NODISCARD inline bool InOldKeys(const KeyCode key) const { return m_OldKeyboardKeys.find(key) != m_OldKeyboardKeys.end(); }

        NODISCARD inline bool InMouseButtons(const MouseButton button) const { return m_MouseButtons.find(button) != m_MouseButtons.end(); }
        NODISCARD inline bool InOldMouseButtons(const MouseButton button) const { return m_OldMouseButtons.find(button) != m_OldMouseButtons.end(); }

        std::unordered_set<KeyCode> m_KeyboardKeys;
        std::unordered_set<KeyCode> m_OldKeyboardKeys;

        std::unordered_set<MouseButton> m_MouseButtons;
        std::unordered_set<MouseButton> m_OldMouseButtons;

        glm::vec2 m_MousePosition = {};
        glm::vec2 m_OldMousePosition = {};
        glm::vec2 m_MouseDelta = {};

        GLFWwindow* m_GLFWWindow = nullptr;

        friend class Input;
        friend void key_action_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        friend void mouse_move_callback(GLFWwindow* window, double xPos, double yPos);
        friend void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    };
}