#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <memory>

namespace Rigel
{
    class WindowManager;
    class AssetManager;
}

namespace Rigel::Backend::Vulkan
{
    class VK_Instance;
    class VK_Device;
    class VK_Surface;
    class VK_Swapchain;
    class VK_Fence;
    class VK_Semaphore;
    class VK_CmdBuffer;
    class VK_GraphicsPipeline;
    class VK_VertexBuffer;
    class VK_IndexBuffer;
    class VK_UniformBuffer;
    class VK_DescriptorSet;
    class VK_DescriptorPool;
    class VK_Image;

    class VK_ImGUI_Renderer;
    class VK_BindlessManager;

    struct AcquireImageInfo;

    class VK_Renderer final
    {
    public:
        VK_Renderer();
        ~VK_Renderer();

        ErrorCode Startup();
        ErrorCode Shutdown();
        ErrorCode LateStartup();

        void Render();
        void WaitForFinish() const;

        inline void SetImGuiBackend(VK_ImGUI_Renderer* backend) { m_ImGuiBackend = backend; }

        NODISCARD inline VK_Device& GetDevice() const { return *m_Device; }
        NODISCARD inline VK_Instance& GetInstance() const { return *m_Instance; }
        NODISCARD inline VK_Swapchain& GetSwapchain() const { return *m_Swapchain; }

        NODISCARD inline VK_BindlessManager& GetBindlessManager() const { return *m_BindlessManager; }
    private:
        void RecordCommandBuffer(const VK_CmdBuffer& commandBuffer, const AcquireImageInfo& image);
        void RecreateSwapchain();
        void CreateDepthBufferImage(const glm::uvec2 size);

        void RenderScene(VkCommandBuffer cmdBuffer);

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;
        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::unique_ptr<VK_Image> m_DepthBufferImage;
        std::unique_ptr<VK_BindlessManager> m_BindlessManager;

        std::vector<std::unique_ptr<VK_Fence>> m_InFlightFences;
        std::vector<std::unique_ptr<VK_Semaphore>> m_RenderFinishedSemaphore;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;
    };
}
