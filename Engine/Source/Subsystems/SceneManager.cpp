#include "SceneManager.hpp"
#include "SceneHandle.hpp"
#include "Assert.hpp"
#include "Debug.hpp"
#include "Logger.hpp"
#include "Scene.hpp"

#include <stdexcept>
#include <utility>

namespace rge
{
    SceneManager::SceneManager() : m_LoadedScene(SceneHandle(nullptr, NULL_ID)) { Startup(); }
    SceneManager::~SceneManager() { Shutdown(); }

    void SceneManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up scene manager.");
    }

    void SceneManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down scene manager.");
    }

    SceneHandle SceneManager::CreateScene(std::string name)
    {
        const auto scene = new Scene(GetNextSceneID(), std::move(name));
        m_Scenes[scene->GetID()] = std::unique_ptr<Scene>(scene);

        return {scene, scene->GetID()};
    }

    void SceneManager::DestroyScene(const SceneHandle& scene)
    {
        if (!IsSceneHandleValid(scene))
        {
            Debug::Error("An invalid handle was passed to SceneManager::DestroyScene!");
            return;
        }

        if (scene->IsLoaded())
        {
            Debug::Error("Attempted to destroy a loaded scene, unload it and then try to destroy it again!");
            return;
        }

        m_Scenes[scene.GetID()].reset();
        m_Scenes.erase(scene.GetID());
    }

    void SceneManager::LoadScene(SceneHandle& scene)
    {
        if (scene.IsNull() || !IsSceneHandleValid(scene))
            throw std::runtime_error("Attempted to load an invalid scene.");

        ASSERT(m_LoadedScene.GetID() != scene.GetID(), "Attempted to load the scene that's already been loaded.");

        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        Debug::VerboseMessage("Loading scene: " + scene->GetName());

        scene->OnLoad();
        m_LoadedScene = scene;
    }

    SceneHandle SceneManager::GetSceneByID(const uid_t id) const
    {
        const auto it = m_Scenes.find(id);
        if (it == m_Scenes.end())
        {
            Debug::Error("Cannot find a valid scene by specified ID: " + std::to_string(id));
            return {nullptr, NULL_ID };
        }

        return {it->second.get(), id};
    }

    SceneHandle SceneManager::GetLoadedScene() const
    {
        ASSERT(IsSceneLoaded(), "No scene is currently loaded.");
        return m_LoadedScene;
    }

    bool SceneManager::IsSceneHandleValid(const SceneHandle& handle) const
    {
        if (const auto it = m_Scenes.find(handle.GetID()); it != m_Scenes.end())
            return true;
        return false;
    }
}
