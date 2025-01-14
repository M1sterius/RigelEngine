#include <utility>

#include "ECS/Scene.hpp"
#include "SceneManager.hpp"
#include "GameObject.hpp"
#include "UIDGenerator.hpp"

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
    bool SceneHandle::IsValid() const
    {
        const auto& sceneManager = rge::SceneManager::Get();
        return sceneManager.IsSceneHandleValid(*this);
    }
    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ObjectID == NULL_ID;
    }

    bool SceneHandle::operator == (const RigelHandle& other) const
    {
        auto otherHandle = dynamic_cast<const SceneHandle*>(&other);
        if (!otherHandle) return false; // Different types

        return m_Ptr == otherHandle->m_Ptr && m_ObjectID == otherHandle->m_ObjectID;
    }

    bool SceneHandle::operator != (const RigelHandle& other) const
    {
        return !(*this == other);
    }
}
