#pragma once

#include "RigelSubsystem.hpp"

namespace rge
{
    class Engine;

    class Renderer final : public RigelSubsystem
    {
    public:
    INTERNAL:
        void Startup() override;
        void Shutdown() override;
    private:
        Renderer() = default;
        ~Renderer() = default;

        friend class Engine;
    };
}