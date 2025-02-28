#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge
{
    class VulkanRenderer final : public RigelSubsystem
    {
    INTERNAL:
        VulkanRenderer();
        ~VulkanRenderer();
    private:
        void Startup() override;
        void Shutdown() override;
    };

}
