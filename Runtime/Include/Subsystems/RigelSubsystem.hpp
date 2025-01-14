#pragma once

#include <mutex>
#include "Utils/NODISCARD.hpp"
#include "Utils/RGE_API.hpp"

namespace rge
{
    template<typename T>
    class RigelSubsystem
    {
    public:
        RigelSubsystem(const RigelSubsystem&) = delete;
        RigelSubsystem operator = (const RigelSubsystem&) = delete;

        NODISCARD inline static T& Get()
        {
            static T instance;
            return instance;
        }
    protected:
        RigelSubsystem() = default;
        ~RigelSubsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
