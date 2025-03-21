#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace rge::backend
{
    /**
     * Base class for specific graphics API rendering implementations (such as Vulkan or OpenGL)
     */
    class RenderingBackend : public RigelSubsystem
    {
    public:
        ~RenderingBackend() override = default;

        virtual void InitImGUI() = 0;
        virtual void Render() = 0;
    protected:
        RenderingBackend() = default;
        void Startup() override = 0;
        void Shutdown() override = 0;
    };
}
