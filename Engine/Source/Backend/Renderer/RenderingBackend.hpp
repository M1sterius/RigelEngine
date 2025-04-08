#pragma once

#include <Renderer.hpp>

#include "Core.hpp"
#include "RigelSubsystem.hpp"

namespace Rigel::Backend
{
    struct SceneRenderInfo;

    /**
     * Base class for specific graphics API rendering implementations (such as Vulkan or OpenGL)
     */
    class RenderingBackend : public RigelSubsystem
    {
    public:
        ~RenderingBackend() override = default;

        virtual void LateInit() = 0;
        virtual void InitImGUI() = 0;
        virtual void Render(SceneRenderInfo& sceneRenderInfo) = 0;
    protected:
        RenderingBackend() = default;
        void Startup() override = 0;
        void Shutdown() override = 0;
    };
}
