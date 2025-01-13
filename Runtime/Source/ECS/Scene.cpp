#include <utility>

#include "ECS/Scene.hpp"
#include "SceneManager.hpp"
#include "GameObject.hpp"
#include "UIdGenerator.hpp"

namespace rge
{
    Scene::Scene(std::string name)
        : RigelObject(), m_Name(std::move(name))
    {
        m_ID = UIDGenerator::Generate();
    }
    Scene::~Scene() = default;

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

    SceneHandle::SceneHandle(Scene* ptr, const uid_t id)
        :   m_Ptr(ptr), m_ID(id) { }

    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID;
    }

    bool SceneHandle::IsValid() const
    {
        return false;
    }

    SceneHandle SceneHandle::NULL_HANDLE()
    {
        return {nullptr, NULL_ID};
    }
}
