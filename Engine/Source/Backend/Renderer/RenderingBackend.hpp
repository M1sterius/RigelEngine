#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge::backend
{
    /**
     * Base class for specific graphics API rendering implementations
     */
    class RenderingBackend : public RigelSubsystem
    {
    public:
        virtual ~RenderingBackend() = default;
    protected:
        RenderingBackend() = default;
        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
