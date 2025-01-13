#pragma once

#include "RigelSubsystem.hpp"
#include "Utils/Types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace rge
{
    class Scene;
    class SceneHandle;

    class SceneManager final : public RigelSubsystem<SceneManager>
    {
    public:
        friend class RigelSubsystem<SceneManager>;

        NODISCARD SceneHandle CreateScene(const std::string& name = "New Scene");
        void LoadScene(const uid_t sceneID);
    INTERNAL:
        void Startup() override;
        void Shutdown() override;

        std::vector<Scene*> m_Scenes;
    };
}
