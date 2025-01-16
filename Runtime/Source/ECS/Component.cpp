#include "ECS/Component.hpp"

namespace rge
{
    Component::Component()
        :   RigelObject()
    {

    }

    bool ComponentHandle::IsValid() const
    {
        return false;
    }

    bool ComponentHandle::IsNull() const
    {
        return false;
    }

    bool ComponentHandle::operator==(const RigelHandle& other) const
    {
        return false;
    }

    bool ComponentHandle::operator!=(const RigelHandle& other) const
    {
        return false;
    }

    ComponentHandle::ComponentHandle(Component* ptr, const uid_t id, const uid_t objectID)
        :   RigelHandle(ptr, id), m_GameObjectID(objectID)
    {

    }
}
