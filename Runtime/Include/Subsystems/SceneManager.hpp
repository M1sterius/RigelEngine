#pragma once

#include "RigelSubsystem.hpp"
#include "Utils/Types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    class Scene;
    class SceneHandle;

    class SceneManager final : public RigelSubsystem<SceneManager>
    {
    public:
        friend class RigelSubsystem<SceneManager>;

        NODISCARD SceneHandle CreateScene(const std::string& name = "New Scene");
        NODISCARD SceneHandle GetSceneByID(const uid_t id);
    INTERNAL:
        void Startup() override;
        void Shutdown() override;
    };
}
