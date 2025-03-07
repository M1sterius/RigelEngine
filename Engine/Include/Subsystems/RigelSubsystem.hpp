#pragma once

#include "Core.hpp"

#include <mutex>

namespace rge
{
    class RigelSubsystem
    {
    public:
        RigelSubsystem(const RigelSubsystem&) = delete;
        RigelSubsystem operator = (const RigelSubsystem&) = delete;

        NODISCARD bool IsInitialized() const { return m_Initialized; }
    protected:
        RigelSubsystem() = default;
        virtual ~RigelSubsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;

        bool m_Initialized = false;
    };
}
