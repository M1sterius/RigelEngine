#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "ComponentHandle.hpp"
#include "Camera.hpp"
#include "ModelRenderer.hpp"

#include <memory>
#include <vector>

namespace rge
{
    namespace backend
    {
        class RenderingBackend;

        struct SceneRenderInfo
        {
            ComponentHandle<Camera> MainCamera;
            std::vector<ComponentHandle<ModelRenderer>> Models;

            inline void Reset()
            {
                MainCamera = ComponentHandle<Camera>::Null();
                Models.clear();
            }
        };
    }

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

        void LateInit() const;

        void Prepare();
        void Render() const;
    private:
        void Startup() override;
        void Shutdown() override;

        backend::SceneRenderInfo m_CurrentRenderInfo;

        NODISCARD GraphicsApi SelectGraphicsAPI();
        GraphicsApi m_SelectedGraphicsAPI = GraphicsApi::None;
        std::unique_ptr<backend::RenderingBackend> m_BackendRenderer;
    };
}
