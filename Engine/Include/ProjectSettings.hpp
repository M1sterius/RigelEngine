#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ISerializable.hpp"

#include "json.hpp"

namespace Rigel
{
    /**
     * A collection of global engine settings and config values
     */
    class ProjectSettings : public ISerializable
    {
    public:
        // Arguments
        int32_t argc = 0;
        char** argv = nullptr;

        // Time
        uint32_t TargetFPS = 120;
        uint32_t PhysicsTickRate = 50;
        float64_t TimeScale = 1.0;

        // Windowing
        glm::uvec2 WindowSize = {1600, 900};

        // Tags and Layers
        // Assets

        NODISCARD nlohmann::json Serialize() const override
        {
            return { };
        }

        bool Deserialize(const nlohmann::json& json) override
        {
            return true;
        }
    };
}