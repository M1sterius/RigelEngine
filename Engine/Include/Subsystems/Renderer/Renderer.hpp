#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include <memory>

namespace rge::backend
{
    class RenderingBackend;
}

namespace rge
{
    enum class GraphicsApi : uint8_t
    {
        None,
        Vulkan,
        OpenGL
    };

    class Renderer final : public RigelSubsystem
    {
    public:
        NODISCARD GraphicsApi GetSelectedGraphicsAPI() const { return m_SelectedGraphicsAPI; }
        NODISCARD static std::string GetGraphicsAPIString(const GraphicsApi api);
    INTERNAL:
        Renderer();
        ~Renderer() override;

        NODISCARD backend::RenderingBackend& GetBackend() const;

        void Render();
    private:
        void Startup() override;
        void Shutdown() override;

        NODISCARD GraphicsApi SelectGraphicsAPI();

        GraphicsApi m_SelectedGraphicsAPI = GraphicsApi::None;
        std::unique_ptr<backend::RenderingBackend> m_BackendRenderer;
    };
}