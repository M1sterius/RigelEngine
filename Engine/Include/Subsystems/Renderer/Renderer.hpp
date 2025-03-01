#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include <memory>

namespace rge
{
    class VK_Renderer;

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

        std::unique_ptr<VK_Renderer> m_VKRenderer;
    };
}