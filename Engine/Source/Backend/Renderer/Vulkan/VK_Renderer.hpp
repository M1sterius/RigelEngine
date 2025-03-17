#pragma once

#include "Core.hpp"
#include "../RenderingBackend.hpp"

#include <memory>

namespace rge
{
    class WindowManager;
}

namespace rge::backend
{
    class VK_Instance;
    class VK_Device;
    class VK_Surface;
    class VK_Swapchain;

    class VK_Renderer final : public RenderingBackend
    {
    public:
        VK_Renderer();
        ~VK_Renderer() override;

        void InitImGUI() override;

        void PrepareFrame() override;
        void RenderScene() override;
        void RenderGizmo() override;
        void RenderGUI() override;
        void FinalizeFrame() override;

        NODISCARD inline VK_Device& GetDevice() const { return *m_Device; }
    private:
        void Startup() override;
        void Shutdown() override;

        void RecreateSwapchain() const;

        uint32_t m_FrameIndex = 0;
        uint32_t m_SwapchainImageIndex = 0;

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;

        WindowManager& m_WindowManager;
    };
}


/*
  Command Buffers & Sync Objects
  Render Pass & Framebuffers
  Graphics Pipeline & Shaders
  Vertex Buffers & Index Buffers
  Descriptor Sets for MVP Matrices
  Render Loop Implementation
  Depth Buffering
  Load & Render a 3D Model
*/