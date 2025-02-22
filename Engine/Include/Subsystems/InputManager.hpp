#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "KeysAndButtons.hpp"

struct GLFWwindow;

namespace rge
{
    class InputManager final : public RigelSubsystem
    {
    INTERNAL:
        InputManager();
        ~InputManager();
    private:
        void Startup() override;
        void Shutdown() override;

        void OnInputUpdate();

        glm::vec2 m_MousePosition = {};
        glm::vec2 m_MouseDelta = {};
        GLFWwindow* m_GLFWWindow = nullptr;

        friend void key_action_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        friend void mouse_move_callback(GLFWwindow* window, double xPos, double yPos);
        friend void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    };
}