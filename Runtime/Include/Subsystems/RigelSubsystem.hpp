#pragma once

#include <mutex>
#include "Utils/NODISCARD.hpp"
#include "Utils/RGE_API.hpp"

namespace rge
{
    class RigelSubsystem
    {
    public:
        RigelSubsystem(const RigelSubsystem&) = delete;
        RigelSubsystem operator = (const RigelSubsystem&) = delete;
    protected:
        RigelSubsystem() = default;
        ~RigelSubsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
