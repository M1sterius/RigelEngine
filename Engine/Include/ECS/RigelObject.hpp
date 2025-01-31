#pragma once

#include "Utils/HeaderUtils.hpp"

namespace rge
{
    class RigelObject
    {
    public:
        RigelObject(const RigelObject&) = delete;
        RigelObject operator = (const RigelObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    protected:
        RigelObject();
        ~RigelObject() = default;

        uid_t m_ID = NULL_ID;
    };
}