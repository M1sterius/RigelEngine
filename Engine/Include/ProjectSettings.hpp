#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ISerializable.hpp"

#include "json.hpp"

#include <string>

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

        // Game
        std::string GameName = "None";
        uint32_t GameVersion = 0;

        // Time
        uint32_t TargetFPS = 120;
        uint32_t PhysicsTickrate = 50;
        float64_t TimeScale = 1.0;

        // Windowing
        glm::uvec2 WindowSize = {1600, 900};
        std::string WindowTitle = "None";

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