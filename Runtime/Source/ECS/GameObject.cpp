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
    bool GOHandle::operator == (const RigelHandle& other) const
    {
        auto otherHandle = dynamic_cast<const GOHandle*>(&other);
        if (!otherHandle) return false; // Different types

        return m_Ptr == otherHandle->m_Ptr && m_ObjectID == otherHandle->m_ObjectID;
    }
    bool GOHandle::operator != (const RigelHandle& other) const
    {
        return !(*this == other);
    }
}
