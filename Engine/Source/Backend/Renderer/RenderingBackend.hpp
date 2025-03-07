#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge::backend
{
    /**
     * Base class for specific graphics API rendering implementations (such as Vulkan and OpenGL)
     */
    class RenderingBackend : public RigelSubsystem
    {
    public:
        ~RenderingBackend() override = default;
    protected:
        RenderingBackend() = default;
        void Startup() override = 0;
        void Shutdown() override = 0;
    };
}
