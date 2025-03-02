#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge_bcknd
{
class VK_Renderer final : public rge::RigelSubsystem
    {
    INTERNAL:
        VK_Renderer();
        ~VK_Renderer();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}
