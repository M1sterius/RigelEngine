#include "SceneManager.hpp"
#include "SceneHandle.hpp"
#include "Assert.hpp"
#include "Debug.hpp"
#include "Scene.hpp"

#include <utility>


namespace Rigel
{
    using namespace Backend::HandleValidation;

    SceneManager::SceneManager() = default;
    SceneManager::~SceneManager() = default;

    ErrorCode SceneManager::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up scene manager.");

        m_Initialized = true;
        return ErrorCode::OK;
    }

    ErrorCode SceneManager::Shutdown()
    {
        m_Scenes.clear();

        Debug::Trace("Shutting down scene manager.");
        return ErrorCode::OK;
    }

    void SceneManager::UnloadCurrentScene()
    {
        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();
    }

    SceneHandle SceneManager::CreateScene(std::string name)
    {
        const auto scene = new Scene(GetNextSceneID(), std::move(name));
        m_Scenes[scene->GetID()] = std::unique_ptr<Scene>(scene);
        HandleValidator::AddHandle<HandleType::SceneHandle>(scene->GetID());

        return {scene, scene->GetID()};
    }

    void SceneManager::DestroyScene(const SceneHandle& scene)
    {
        if (scene->IsLoaded())
        {
            Debug::Error("Attempted to destroy a loaded scene, unload it and then try to destroy it again!");
            return;
        }

        const auto id = scene.GetID();
        HandleValidator::RemoveHandle<HandleType::SceneHandle>(id);
        m_Scenes.erase(id);
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
