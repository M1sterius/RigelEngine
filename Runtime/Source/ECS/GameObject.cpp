#include "ECS/GameObject.hpp"
#include "SceneManager.hpp"
#include "ECS/Scene.hpp"

namespace rge
{

    GameObject::GameObject(std::string name)
        :   m_Name(std::move(name)), m_ID(0)
    {

    }

    GameObject::~GameObject() = default;


    GOHandle::GOHandle(GameObject* ptr, const uid_t sceneID, const uid_t goID)
        :   m_Ptr(ptr), m_SceneID(sceneID), m_GameObjectID(goID) { }

    bool GOHandle::IsValid() const
    {
        auto scene = rge::SceneManager::Get().GetSceneByID(m_SceneID);
        return scene->CheckGOValidity(m_GameObjectID);
    }

    GOHandle GOHandle::NULL_HANDLE()
    {
        return {nullptr, NULL_ID, NULL_ID};
    }

    bool GOHandle::IsNull() const
    {
        return m_Ptr == nullptr && m_GameObjectID == NULL_ID && m_SceneID == NULL_ID;
    }
}
