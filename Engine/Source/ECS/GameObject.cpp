#include "GameObject.hpp"
#include "UIDGenerator.hpp"
#include "Engine.hpp"
#include "SceneManager.hpp"

namespace rge
{

    GameObject::GameObject(std::string name)
    {
        m_ID = UIDGenerator::Generate();
        m_Name = std::move(name);
    }

    GameObject::~GameObject()
    {

    }

    // Handle methods definitions
    GOHandle::GOHandle(GameObject* ptr, const uid_t id, const uid_t sceneID) : RigelHandle(ptr, id), m_SceneID(sceneID) { }

    bool GOHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID || m_SceneID == NULL_ID;
    }

    bool GOHandle::IsValid() const
    {
        const auto& sceneManager = Engine::Get().GetSceneManager();
        const auto scene = sceneManager.GetSceneByID(m_SceneID);
        return scene->IsGOHandleValid(*this);
    }
}