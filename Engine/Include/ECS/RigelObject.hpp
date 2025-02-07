#pragma once

#include "Core.hpp"

namespace rge
{
    /**
     * Base class for objects managed by the engine and it's subsystems
     */
    class RigelObject
    {
    public:
        RigelObject(const RigelObject&) = delete;
        RigelObject operator = (const RigelObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    protected:
        explicit inline RigelObject(const uid_t id) : m_ID(id) { }
        virtual ~RigelObject() = default;
    private:
        const uid_t m_ID = NULL_ID;
    };
}
