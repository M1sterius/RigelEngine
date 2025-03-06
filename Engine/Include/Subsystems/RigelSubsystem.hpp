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
    protected:
        RigelSubsystem() = default;
        virtual ~RigelSubsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
