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

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        void Tick();
    private:
        float64_t m_Accumulator = 0.0;
        float64_t m_TickTime = 0.05;
    };
}
