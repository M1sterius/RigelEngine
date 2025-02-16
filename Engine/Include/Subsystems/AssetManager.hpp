#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge
{
    class AssetManager final : public RigelSubsystem
    {
    public:
    INTERNAL:
        AssetManager();
        ~AssetManager();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}