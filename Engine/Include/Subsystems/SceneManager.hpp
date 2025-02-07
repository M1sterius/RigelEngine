#pragma once

#include "RigelSubsystem.hpp"
#include "Core.hpp"
#include "SceneHandle.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    /**
    * Core subsystem for managing scenes, their creation, usage and lifetime
    *
    * Initialized during engine startup stage, an instance can be obtained by calling
    * an appropriate Engine::GetSceneManager() member function
    */
    class SceneManager final : public RigelSubsystem
    {
    public:
        NODISCARD SceneHandle CreateScene(std::string name = "New Scene");
        NODISCARD SceneHandle GetSceneByID(const uid_t id) const;

        NODISCARD bool IsSceneLoaded() const { return !m_LoadedScene.IsNull(); }
        NODISCARD bool IsSceneHandleValid(const SceneHandle& handle) const;
        NODISCARD SceneHandle GetLoadedScene() const;

        void LoadScene(SceneHandle& scene);
    INTERNAL:
        SceneManager();
        ~SceneManager();
    private:
        void Startup() override;
        void Shutdown() override;

        uid_t m_NextSceneID = 1;
        std::unordered_map<uid_t, Scene*> m_Scenes;
        SceneHandle m_LoadedScene;
    };
}
