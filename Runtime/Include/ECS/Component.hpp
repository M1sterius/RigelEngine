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

    class ComponentHandle final : public RigelHandle<Component>
    {
    public:
        NODISCARD bool IsValid() const override;
        NODISCARD bool IsNull() const override;
        NODISCARD inline uid_t GetGameObjectID() const { return m_GameObjectID; }

        bool operator == (const RigelHandle& other) const override;
        bool operator != (const RigelHandle& other) const override;
    INTERNAL:
        ComponentHandle(Component* ptr, const uid_t id, const uid_t objectID);
    private:
        uid_t m_GameObjectID = NULL_ID;
    };
}
