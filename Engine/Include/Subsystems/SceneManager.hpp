#pragma once

#include <json.hpp>

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
        /**
         * Creates a new instance of an empty scene.
         * @param name Optional name for the scene, does not uniquely identify it.
         * @return Handle to the scene that will be created.
         */
        NODISCARD SceneHandle CreateScene(std::string name = "New Scene");

        /**
         * Deletes a scene and all objects on it. On an unloaded scene can be deleted.
         * @param scene Handle to the scene that will be deleted.
         */
        void DestroyScene(const SceneHandle& scene);

        /**
         * Find a scene by its unique ID.
         * @param id
         * @return Handle to the scene that was found. Will return a null handle if the scene cannot be found.
         */
        NODISCARD SceneHandle GetSceneByID(const uid_t id) const;

        NODISCARD bool IsSceneLoaded() const { return !m_LoadedScene.IsNull(); }
        NODISCARD bool IsSceneHandleValid(const SceneHandle& handle) const;
        NODISCARD SceneHandle GetLoadedScene() const;

        void LoadScene(SceneHandle& scene);
    INTERNAL:
        SceneManager();
        ~SceneManager() override;
    private:
        void Startup() override;
        void Shutdown() override;

        NODISCARD inline uid_t GetNextSceneID() { return m_NextSceneID++; }

        uid_t m_NextSceneID = 1;
        std::unordered_map<uid_t, std::unique_ptr<Scene>> m_Scenes; // The scene manager owns all scenes
        SceneHandle m_LoadedScene;
    };
}
