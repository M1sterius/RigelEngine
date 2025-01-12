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

        m_Objects[id] = go;

        return {go, m_ID, id};
    }

    bool Scene::CheckGOValidity(const uid_t id)
    {
        auto it = m_Objects.find(id);
        if (it != m_Objects.end())
            return true;
        return false;
    }

    Scene::~Scene() = default;

    void SceneManager::LoadScene(const uid_t id)
    {
        auto scene = GetScene(id);
        LoadScene(scene);
    }

    SceneHandle::SceneHandle(Scene* ptr, const uid_t id)
        :   m_Ptr(ptr), m_ID(id)
    {

    }

    bool SceneHandle::IsValid()
    {
        return false;
    }
}
