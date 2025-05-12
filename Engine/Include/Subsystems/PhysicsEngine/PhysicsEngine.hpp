#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel
{
    class ProjectSettings;

    class PhysicsEngine final : public RigelSubsystem
    {
    INTERNAL:
        PhysicsEngine() = default;
        ~PhysicsEngine() override = default;

        int32_t Startup(const ProjectSettings& settings) override;
        int32_t Shutdown() override;

        void Tick();
    private:
        float64_t m_Accumulator = 0.0;
        float64_t m_TickTime = 0.05;
    };
}
