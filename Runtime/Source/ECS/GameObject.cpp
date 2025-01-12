#include "ECS/GameObject.hpp"

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
        return false;
    }


}
