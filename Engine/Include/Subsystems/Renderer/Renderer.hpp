#pragma once

#include "../RigelSubsystem.hpp"

namespace rge
{
    class Engine;

    class Renderer final : public RigelSubsystem
    {
    public:
    INTERNAL:
        Renderer();
        ~Renderer();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}