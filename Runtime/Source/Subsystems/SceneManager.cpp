#include "SceneManager.hpp"
#include "Engine.hpp"
#include "RGE_PCH.hpp"
#include "Scene.hpp"
#include "UIdGenerator.hpp"

#include <stdexcept>

namespace rge
{
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
        m_Scenes.push_back(scene);

        return {scene, scene->GetID()};
    }

    void SceneManager::LoadScene(const uid_t sceneID)
    {

    }
}
