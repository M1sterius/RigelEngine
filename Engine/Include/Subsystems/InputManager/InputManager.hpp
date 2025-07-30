#pragma once

#include "Core.hpp"
#include "../RigelSubsystem.hpp"
#include "KeysAndButtons.hpp"

#include "Math.hpp"

#include <unordered_set>

struct GLFWwindow;

namespace Rigel
{
    class ProjectSettings;

    class InputManager final : public RigelSubsystem
    {
    public:
        NODISCARD bool GetKeyDown(const KeyCode key) const;
        NODISCARD bool GetKeyPressed(const KeyCode key) const;
        NODISCARD bool GetKeyUp(const KeyCode key) const;

        NODISCARD bool GetMouseButtonDown(const MouseButton button) const;
        NODISCARD bool GetMouseButtonPressed(const MouseButton button) const;
        NODISCARD bool GetMouseButtonUp(const MouseButton button) const;

        NODISCARD glm::vec2 GetMousePosition() const;
        NODISCARD glm::vec2 GetMouseDelta() const;
    INTERNAL:
        InputManager() = default;
        ~InputManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        void ResetInputState();
    private:
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
