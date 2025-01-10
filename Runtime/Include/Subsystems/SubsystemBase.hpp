#pragma once

#include "NODISCARD.hpp"

namespace rge
{
    class Core;

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
