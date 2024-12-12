#pragma once

#include "Internal.hpp"

namespace rge
{
    /**
     * @brief Base class for all Rigel engine subsystems
     */
    class Subsystem
    {
    public:
        Subsystem(const Subsystem& other) = delete;
        Subsystem& operator =(Subsystem&&) = delete;
    INTERNAL:
        Subsystem() = default;
        virtual ~Subsystem() = default;

        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
