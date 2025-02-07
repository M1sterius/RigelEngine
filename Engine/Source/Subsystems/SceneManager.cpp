#include "SceneManager.hpp"
#include "SceneHandle.hpp"
#include "Assert.hpp"
#include "Debug.hpp"
#include "Scene.hpp"

#include <stdexcept>
#include <utility>

namespace rge
{
    SceneManager::SceneManager() : m_LoadedScene(SceneHandle(nullptr, NULL_ID)) { Startup(); }
    SceneManager::~SceneManager() { Shutdown(); }

    void SceneManager::Startup()
    {

    }

    void SceneManager::Shutdown()
    {

    }

    SceneHandle SceneManager::CreateScene(std::string name)
    {
        const auto scene = new Scene(m_NextSceneID++, std::move(name));
        m_Scenes[scene->GetID()] = scene;

        return {scene, scene->GetID()};
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
            throw std::runtime_error("Attempted to retrieve a scene by an invalid ID.");

        return {it->second, id};
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
        else return false;
    }
}
