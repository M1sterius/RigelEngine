#pragma once

#include "RigelSubsystem.hpp"

namespace rge
{
    enum class GraphicsApi : uint8_t
    {
        Default = 0,
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

        GraphicsApi ChooseGraphicsAPI() const;

        GraphicsApi m_GraphicsAPI = GraphicsApi::Default;
    };
}