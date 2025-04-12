#pragma once

#include "Core.hpp"
#include "Renderer.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel::Backend
{
    /**
     * Base class for specific graphics API renderer implementations (such as Vulkan or OpenGL)
     */
    class IRendererBackend
    {
    public:
        virtual ~IRendererBackend() = default;

        IRendererBackend(const IRendererBackend& other) = delete;
        IRendererBackend& operator = (const IRendererBackend&) = delete;

        virtual void LateInit() = 0;
        virtual void Render() = 0;
        virtual void WaitForFinish() const = 0;
    protected:
        IRendererBackend() = default;
        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
