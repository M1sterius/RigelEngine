#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

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

        GLFWwindow* m_GLFWwindow = nullptr;
    };
}