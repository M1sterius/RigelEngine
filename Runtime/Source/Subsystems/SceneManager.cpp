#include "SceneManager.hpp"
#include "Engine.hpp"
#include "RGE_PCH.hpp"
#include "Scene.hpp"
#include "UIdGenerator.hpp"

#include <stdexcept>

namespace rge
{
    SceneManager& SceneManager::Get()
    {
        static auto instance = SceneManager();
        return instance;
    }

    void SceneManager::Startup()
    {
        rge::Logger::VerboseMessage("SceneManager successfully initialized.");
    }

    void SceneManager::Shutdown()
    {
        rge::Logger::VerboseMessage("SceneManager successfully shutdown.");
    }

    std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name)
    {
        auto id = UIDGenerator::Generate();
        auto scene = std::make_shared<Scene>(name, id);

        m_Scenes[id] = scene;
        return scene;
    }

    std::shared_ptr<Scene> SceneManager::GetScene(const uid_t id)
    {
        auto it = m_Scenes.find(id);
        if (it != m_Scenes.end())
            return it->second;

        throw std::runtime_error("Cannot find the scene by the specified ID!");
    }

    void SceneManager::LoadScene(const std::shared_ptr<Scene>& scene)
    {
        if (IsSceneLoaded())
            m_LoadedScene->OnUnload();

        m_LoadedScene = scene;
        m_LoadedScene->OnLoad();
    }
}
