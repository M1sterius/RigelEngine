#include "ECS/GameObject.hpp"
#include "SceneManager.hpp"
#include "ECS/Scene.hpp"
#include "UIDGenerator.hpp"
#include "Time.hpp"

namespace rge
{
    GameObject::GameObject(std::string name)
        :   RigelObject(), m_Name(std::move(name))
    {
        m_ID = UIDGenerator::Generate();
    }
    GameObject::~GameObject() = default;

    void GameObject::OnLoad()
    {
        for (auto& [ _, ptr] : m_Components)
            ptr->Load();
        for (auto& [ _, ptr] : m_Components)
            ptr->Start();
    }

    void GameObject::OnUnload()
    {
        for (auto& [ _, ptr] : m_Components)
            ptr->OnDestroy();
    }

    void GameObject::OnGameUpdate()
    {
        for (auto& [ _, ptr] : m_Components)
            ptr->Update();
    }

    void GameObject::SetActive(const bool active)
    {
        if (m_IsActive == false && active == true)
            OnEnable();
        else if (m_IsActive == true && active == false)
            OnDisable();
        m_IsActive = active;
    }

    void GameObject::OnEnable()
    {
        for (auto& [ _, ptr] : m_Components)
            ptr->OnEnable();
    }

    void GameObject::OnDisable()
    {
        for (auto& [ _, ptr] : m_Components)
            ptr->OnDisable();
    }

    // Handle
    GOHandle::GOHandle(GameObject* ptr, const uid_t id) : RigelHandle(ptr, id) { }
    bool GOHandle::IsValid() const
    {
        return false;
    }
    bool GOHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ObjectID == NULL_ID;
    }
}
