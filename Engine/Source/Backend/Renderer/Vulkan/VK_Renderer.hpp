#pragma once

#include "Core.hpp"
#include "../RenderingBackend.hpp"
#include "vulkan.h"

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
    class VK_Fence;
    class VK_Semaphore;
    class VK_CmdBuffer;

    class VK_Renderer final : public RenderingBackend
    {
    public:
        VK_Renderer();
        ~VK_Renderer() override;

        void InitImGUI() override;
        void Render() override;

        NODISCARD inline VK_Device& GetDevice() const { return *m_Device; }
    private:
        void Startup() override;
        void Shutdown() override;

        void RecreateSwapchain() const;
        NODISCARD std::pair<VkPipeline, VkPipelineLayout> CreateGraphicsPipeline() const;

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;

        std::vector<std::unique_ptr<VK_Fence>> m_InFlightFences;
        std::vector<std::unique_ptr<VK_Semaphore>> m_RenderFinishedSemaphore;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;

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