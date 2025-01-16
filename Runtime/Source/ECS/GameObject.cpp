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

    }

    void GameObject::OnUnload()
    {

    }

    void GameObject::OnGameUpdate()
    {
        printf("Frame: %zu, Delta time: %.4f\n", rge::Time::GetFrameCount(), rge::Time::GetDeltaTime());
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

    }

    void GameObject::OnDisable()
    {

    }

//    ComponentHandle GameObject::ConstructComponentHandle(Component* ptr, const uid_t id)
//    {
//        return {ptr, id, m_ID};
//    }

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
