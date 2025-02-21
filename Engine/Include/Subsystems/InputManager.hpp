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

        GLFWwindow* m_GLFWWindow = nullptr;
    };
}