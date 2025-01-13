#pragma once

#include "RigelSubsystem.hpp"
#include "Utils/Types.hpp"
#include "ECS/Scene.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    class SceneManager final : public RigelSubsystem<SceneManager>
    {
    public:
        friend class RigelSubsystem<SceneManager>;

        NODISCARD SceneHandle CreateScene(const std::string& name = "New Scene");
        NODISCARD SceneHandle GetSceneByID(const uid_t id) const;

        NODISCARD bool IsSceneLoaded() const { return !m_LoadedScene.IsNull(); }
        NODISCARD SceneHandle GetLoadedScene() const;

        void LoadScene(SceneHandle& scene);
    INTERNAL:
        void Startup() override;
        void Shutdown() override;

        std::unordered_map<uid_t, Scene*> m_Scenes;
        SceneHandle m_LoadedScene;
    private:
        SceneManager();
        ~SceneManager() = default;
    };
}
