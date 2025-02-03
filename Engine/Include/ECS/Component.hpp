#pragma once

#include "ISerializable.hpp"
#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "Scene.hpp"

namespace rge
{
    class Component
    {
    public:
        Component(const Component&) = delete;
        Component operator = (const Component&) = delete;
        virtual ~Component() = default;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    protected:
        Component();
    private:
        uid_t m_ID = NULL_ID;

        friend class GameObject;
    };

    template<typename T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID || m_GameObjectID == NULL_ID || m_SceneID == NULL_ID;
        }

        NODISCARD bool IsValid() const override
        {
            return true;
        }

        ComponentHandle(T* ptr, const uid_t id, const uid_t objectID, const uid_t sceneID)
            :   RigelHandle<T>(ptr, id), m_GameObjectID(objectID), m_SceneID(sceneID) { }
    private:
        uid_t m_GameObjectID = NULL_ID;
        uid_t m_SceneID = NULL_ID;
    };
}
