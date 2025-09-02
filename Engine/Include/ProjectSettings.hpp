#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Utilities/Serialization/ISerializable.hpp"
#include "Subsystems/WindowManager/DisplayMode.hpp"

#include "nlohmann_json/json.hpp"

#include <string>
#include <map>

namespace Rigel
{
    /**
     * A collection of global engine settings and config values
     */
    class ProjectSettings : public ISerializable
    {
    public:
        // Console arguments
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
        DisplayMode DisplayMode = DisplayMode::Windowed;
        glm::uvec2 WindowSize = {1280, 720};
        std::string WindowTitle = "None";
        bool WindowResizeable = true;

        // Tags and Layers
        std::map<uid_t, std::string> TagsMap = {
            {0, "Default"},
            {1, "Main Camera"}
        };

        // Assets and asset manager
        uint32_t AssetManagerThreadPoolSize = 4; // set to 0 for std::thread::hardware_concurrency()
        bool EnableAssetLifetimeLogging = true;

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