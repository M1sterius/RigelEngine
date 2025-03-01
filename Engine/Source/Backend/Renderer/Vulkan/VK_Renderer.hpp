#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge
{
    class VK_Renderer final : public RigelSubsystem
    {
    INTERNAL:
        VK_Renderer();
        ~VK_Renderer();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}
