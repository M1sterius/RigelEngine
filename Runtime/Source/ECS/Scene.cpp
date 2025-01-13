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

        m_Objects[go->GetID()] = go;

        return {go, go->GetID()};
    }

    bool Scene::CheckGOValidity(const uid_t id)
    {
        auto it = m_Objects.find(id);
        if (it != m_Objects.end())
            return true;
        return false;
    }

    // Handle
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle<Scene>(ptr, id) { }
    bool SceneHandle::IsValid()
    {
        return true;
    }
    bool SceneHandle::IsNull()
    {
        return m_Ptr == nullptr && m_ObjectID == NULL_ID;
    }
}
