#pragma once

#include "ISerializable.hpp"
#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "Scene.hpp"

namespace rge
{
    class Component : public ISerializable
    {
    public:
        Component(const Component&) = delete;
        Component operator = (const Component&) = delete;
        ~Component() override = default;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    protected:
        Component();
    private:
        uid_t m_ID = NULL_ID;

        friend class GameObject;
    };
}
