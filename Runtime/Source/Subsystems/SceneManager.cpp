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
        auto id = UIDGenerator::Generate();
        auto scene = new Scene(name);

        return {scene, id};
    }

    SceneHandle SceneManager::GetSceneByID(const uid_t id)
    {
//        auto it = m_Scenes.find(id);
//        if (it == m_Scenes.end())
//            return {nullptr, NULL_ID};

        return {nullptr, NULL_ID};
    }
}
