#pragma once

#include "ISerializable.hpp"
#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"

namespace rge
{
    class Component final
    {
    public:
        Component(const Component&) = delete;
        Component operator = (const Component&) = delete;

        virtual ~Component() = default;
    private:
        Component();

        uid_t m_ID = NULL_ID;

        friend class GameObject;
    };

    template<typename T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID || m_GOID == NULL_ID || m_SceneID == NULL_ID;
        }
        NODISCARD bool IsValid() const override
        {
            return true;
        }
    INTERNAL:
        ComponentHandle(T* ptr, const uid_t id, const uid_t goID, const uid_t sceneID);
    private:
        uid_t m_GOID = NULL_ID;
        uid_t m_SceneID = NULL_ID;
    };

    template<typename T>
    ComponentHandle<T>::ComponentHandle(T* ptr, const uid_t id, const uid_t goID, const uid_t sceneID)
        :   RigelHandle<T>(ptr, id), m_GOID(goID), m_SceneID(sceneID)
    {

    }
}
