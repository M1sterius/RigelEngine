#include "SceneManager.hpp"
#include "RGE_PCH.hpp"
#include "UIDGenerator.hpp"

#include <stdexcept>

namespace rge
{
    SceneManager::SceneManager() : RigelSubsystem<SceneManager>(), m_LoadedScene(nullptr, NULL_ID) { }
    void SceneManager::Startup()
    {
        rge::Logger::VerboseMessage("SceneManager successfully initialized.");
    }
    void SceneManager::Shutdown()
    {
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
        if (scene.IsNull())
            throw std::runtime_error("Attempted to load a null scene.");

        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        scene->OnLoad();
        m_LoadedScene = scene;
    }

    SceneHandle SceneManager::GetSceneByID(const uid_t id) const
    {
        auto it = m_Scenes.find(id);
        if (it == m_Scenes.end())
            return {nullptr, NULL_ID};
        return {it->second, id};
    }

    SceneHandle SceneManager::GetLoadedScene() const
    {
        if (!IsSceneLoaded())
            throw std::runtime_error("No scene is currently loaded.");

        return m_LoadedScene;
    }
}
