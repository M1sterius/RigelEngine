#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include <memory>

namespace rge::backend
{
    class VK_Instance;

    class VK_Renderer final : public RigelSubsystem
    {
    INTERNAL:
        VK_Renderer();
        ~VK_Renderer();
    private:
        void Startup() override;
        void Shutdown() override;

        std::unique_ptr<VK_Instance> m_Instance;
    };
}
