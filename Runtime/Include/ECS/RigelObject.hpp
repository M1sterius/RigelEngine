#pragma once

#include "Utils/Types.hpp"
#include "Utils/NODISCARD.hpp"

namespace rge
{
    class RigelObject
    {
    public:
        RigelObject(const RigelObject&) = delete;
        RigelObject operator = (const RigelObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    protected:
        RigelObject() = default;
        ~RigelObject() = default;

        uid_t m_ID = NULL_ID;
    };
}