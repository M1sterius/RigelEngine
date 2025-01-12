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
        :   m_Ptr(ptr), m_SceneID(sceneID), m_GameObjectID(goID)
    {

    }

    bool GOHandle::IsValid() const
    {
        auto scene = rge::SceneManager::Get().GetScene(m_SceneID);
        return scene->CheckGOValidity(m_GameObjectID);
    }
}
