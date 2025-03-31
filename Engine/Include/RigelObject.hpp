#pragma once

#include "Core.hpp"

namespace rge
{
    /**
     * Base class for objects managed by the engine and it's subsystems.
     * Always contains a unique ID assigned to it.
     */
    class RigelObject
    {
    public:
        RigelObject(const RigelObject&) = delete;
        RigelObject operator = (const RigelObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
    INTERNAL:
        inline void OverrideID(const uid_t id) { m_ID = id; }
    protected:
        explicit RigelObject(const uid_t id) : m_ID(id) { }
        virtual ~RigelObject() = default;
    private:
        uid_t m_ID = NULL_ID;
    };
}
