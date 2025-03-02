#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include <memory>

namespace rge_bcknd
{
    class VK_Renderer;
}

namespace rge
{
    enum class GraphicsApi : uint8_t
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

        std::unique_ptr<rge_bcknd::VK_Renderer> m_VKRenderer;
    };
}