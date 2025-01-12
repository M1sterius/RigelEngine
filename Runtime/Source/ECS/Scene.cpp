#include <utility>

#include "ECS/Scene.hpp"
#include "SceneManager.hpp"
#include "GameObject.hpp"
#include "UIdGenerator.hpp"

namespace rge
{
    Scene::Scene(std::string name, const uid_t uid)
        : m_Name(std::move(name)), m_ID(uid)
    {

    }

    void Scene::OnLoad()
    {
        rge::Logger::VerboseMessage("Loading scene: " + m_Name);
    }

    void Scene::OnUnload()
    {
        rge::Logger::VerboseMessage("Unloading scene: " + m_Name);
    }

    GOHandle Scene::AddGameObject()
    {
        auto go = new GameObject("New Game Object");
        auto id = UIDGenerator::Generate();

        return {go, m_ID, id};
    }

    bool Scene::CheckGOValidity(const uid_t id)
    {
        return false;
    }

    Scene::~Scene() = default;
}
