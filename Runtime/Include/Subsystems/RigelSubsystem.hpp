#pragma once

#include "Utils/NODISCARD.hpp"

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
            static auto instance = T();
            return instance;
        }
    protected:
        RigelSubsystem() = default;
        ~RigelSubsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}