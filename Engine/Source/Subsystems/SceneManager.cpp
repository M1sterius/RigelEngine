#include "SceneManager.hpp"
#include "SceneHandle.hpp"
#include "Assert.hpp"
#include "Debug.hpp"
#include "Scene.hpp"

#include <utility>

namespace Rigel
{
    SceneManager::SceneManager() { Startup(); }
    SceneManager::~SceneManager() { Shutdown(); }

    void SceneManager::Startup()
    {
        Debug::Trace("Starting up scene manager.");

        m_Initialized = true;
    }

    void SceneManager::Shutdown()
    {
        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        Debug::Trace("Shutting down scene manager.");
    }

    SceneHandle SceneManager::CreateScene(std::string name)
    {
        const auto scene = new Scene(GetNextSceneID(), std::move(name));
        m_Scenes[scene->GetID()] = std::unique_ptr<Scene>(scene);

        return {scene, scene->GetID()};
    }

    void SceneManager::DestroyScene(const SceneHandle& scene)
    {
        if (scene->IsLoaded())
        {
            Debug::Error("Attempted to destroy a loaded scene, unload it and then try to destroy it again!");
            return;
        }

        m_Scenes.erase(scene.GetID());
    }

    void SceneManager::LoadScene(SceneHandle& scene)
    {
        ASSERT(m_LoadedScene.GetID() != scene.GetID(), "Attempted to load the scene that's already been loaded.");

        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        Debug::Trace("Loading scene: " + scene->GetName());

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
        return m_LoadedScene;
    }
}
