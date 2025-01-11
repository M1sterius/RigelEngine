#pragma once

#include "Utils/NODISCARD.hpp"

namespace rge
{
    class Engine;

    class SubsystemBase
    {
    public:
        SubsystemBase(const SubsystemBase&) = delete;
        SubsystemBase operator = (const SubsystemBase&) = delete;
    INTERNAL:
        virtual void Startup() = 0;
        virtual void Shutdown() = 0;

        SubsystemBase() = default;
        ~SubsystemBase() = default;
    };
}
