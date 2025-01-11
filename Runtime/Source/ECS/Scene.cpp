#include <utility>

#include "ECS/Scene.hpp"
#include "SceneManager.hpp"

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

    Scene::~Scene() = default;
}
