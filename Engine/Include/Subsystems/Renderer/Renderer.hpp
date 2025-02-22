#pragma once

#include "RigelSubsystem.hpp"

namespace rge
{
    enum class GraphicsApi
    {
        Vulkan,
        OpenGL
    };

    class Renderer final : public RigelSubsystem
    {
    public:
    INTERNAL:
        Renderer();
        ~Renderer();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}