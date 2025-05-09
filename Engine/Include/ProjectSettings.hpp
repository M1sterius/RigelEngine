#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace Rigel
{
    class ProjectSettings
    {
    public:
        // Arguments
        int32_t argc = 0;
        char** argv = nullptr;

        // Time
        uint32_t TargetFPS = 120;
        uint32_t PhysicsTickRate = 50;

        // Windowing
        glm::uvec2 WindowSize = {1600, 900};
    };
}