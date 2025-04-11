#pragma once

#include "Core.hpp"
#include "Renderer.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel::Backend
{
    /**
     * Base class for specific graphics API renderer implementations (such as Vulkan or OpenGL)
     */
    class IRendererBackend : public RigelSubsystem
    {
    public:
        ~IRendererBackend() override = default;

        virtual void LateInit() = 0;
        virtual void InitImGUI() = 0;
        virtual void Render() = 0;
        virtual void WaitForFinish() const = 0;
    protected:
        IRendererBackend() = default;
        void Startup() override = 0;
        void Shutdown() override = 0;
    };
}
