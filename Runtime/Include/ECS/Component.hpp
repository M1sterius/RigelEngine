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
    };

    template<typename T>
    class ComponentHandle
    {
    public:
        inline T* operator -> ()
        {
            return m_Ptr;
        }
        ComponentHandle(T* ptr, uid_t id, uid_t goID) : m_Ptr(ptr), m_ID(id), m_GameObjectID(goID) {}
    private:
        T* m_Ptr;
        uid_t m_ID;
        uid_t m_GameObjectID;
    };
}
