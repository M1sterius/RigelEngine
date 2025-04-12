#pragma once

#include "Core.hpp"
#include "IRendererBackend.hpp"

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

    struct AcquireImageInfo;

    class VK_Renderer final : public IRendererBackend
    {
    public:
        VK_Renderer();
        ~VK_Renderer() override;

        void LateInit() override;
        void Render() override;
        void WaitForFinish() const override;

        inline void SetImGuiBackend(VK_ImGUI_Renderer* backend) { m_ImGuiBackend = backend; }

        NODISCARD inline VK_Device& GetDevice() const { return *m_Device; }
        NODISCARD inline VK_Instance& GetInstance() const { return *m_Instance; }
        NODISCARD inline VK_Swapchain& GetSwapchain() const { return *m_Swapchain; }
    private:
        void Startup() override;
        void Shutdown() override;

        void RecordCommandBuffer(VkCommandBuffer commandBuffer, const AcquireImageInfo& image) const;
        void RecreateSwapchain();
        void CreateDepthBufferImage(const glm::uvec2 size);

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;
        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;
        std::unique_ptr<VK_Image> m_DepthBufferImage;

        std::vector<std::unique_ptr<VK_UniformBuffer>> m_UniformBuffers;
        std::vector<std::unique_ptr<VK_DescriptorSet>> m_DescriptorSets;

        std::vector<std::unique_ptr<VK_Fence>> m_InFlightFences;
        std::vector<std::unique_ptr<VK_Semaphore>> m_RenderFinishedSemaphore;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;
        WindowManager& m_WindowManager;
        AssetManager& m_AssetManager;
    };
}
