#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel
{
    class PhysicsEngine final : public RigelSubsystem
    {
    INTERNAL:
        PhysicsEngine();
        ~PhysicsEngine() override;

        void Tick();
    private:
        void Startup() override;
        void Shutdown() override;

        float64_t m_Accumulator = 0.0;
        float64_t m_TickTime = 0.05;
    };
}
