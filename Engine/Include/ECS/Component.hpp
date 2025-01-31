#pragma once

#include "RigelHandle.hpp"
#include "RigelObject.hpp"
#include "Utils/HeaderUtils.hpp"

namespace rge
{
    class RGE_API Component : public RigelObject
    {
    public:
    protected:
        Component();
        virtual ~Component() = default;

        // Event functions
        virtual void Load() { }
        virtual void Start() { }
        virtual void OnEnable() { }
        virtual void OnDisable() { }
        virtual void Update() { }
        virtual void OnDestroy() { }

        friend class GameObject;
    };

    template<typename T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        NODISCARD bool IsValid() const override
        {
            return true;
        }
        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ObjectID == NULL_ID || m_GameObjectID == NULL_ID;
        }

        ComponentHandle(T* ptr, uid_t id, uid_t goID)
            :   RigelHandle<T>(ptr, id), m_GameObjectID(goID) { }
    private:
        uid_t m_GameObjectID = NULL_ID;
    };
}
