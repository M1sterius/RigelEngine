#include "SceneManager.hpp"
#include "RGE_PCH.hpp"
#include "UIDGenerator.hpp"

#include <stdexcept>

namespace rge
{
    SceneManager::SceneManager() : m_LoadedScene(nullptr, NULL_ID) { }
    void SceneManager::Startup()
    {
        rge::Logger::VerboseMessage("SceneManager successfully initialized.");
    }
    void SceneManager::Shutdown()
    {
        for (const auto& scene : m_Scenes)
            delete scene.second;

        rge::Logger::VerboseMessage("SceneManager successfully shutdown.");
    }

    SceneHandle SceneManager::CreateScene(const std::string& name)
    {
        auto scene = new rge::Scene(name);
        m_Scenes[scene->GetID()] = scene;

        return {scene, scene->GetID()};
    }

    void SceneManager::LoadScene(SceneHandle& scene)
    {
        if (scene.IsNull() || !IsSceneHandleValid(scene))
            throw std::runtime_error("Attempted to load an invalid scene.");

        ASSERT(m_LoadedScene != scene, "Attempted to load the scene that's already been loaded.");

        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        scene->OnLoad();
        m_LoadedScene = scene;
    }

    SceneHandle SceneManager::GetSceneByID(const uid_t id) const
    {
        auto it = m_Scenes.find(id);
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
        auto it = m_Scenes.find(handle.GetID());
        if (it != m_Scenes.end())
            return true;
        else return false;
    }
}
