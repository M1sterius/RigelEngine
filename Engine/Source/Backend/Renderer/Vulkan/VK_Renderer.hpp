#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <thread>
#include <unordered_map>

namespace Rigel
{
    class WindowManager;
    class AssetManager;

    namespace Backend
    {
        struct SceneRenderInfo;
    }
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
    class VK_DescriptorSet;
    class VK_DescriptorPool;
    class VK_MemoryBuffer;
    class VK_Image;
    class VK_GBuffer;
    class VK_StagingManager;

    class VK_ImGUI_Renderer;
    class VK_BindlessManager;

    struct AcquireImageInfo;

    class VK_Renderer final
    {
    public:
        VK_Renderer();
        ~VK_Renderer();

        VK_Renderer(const VK_Renderer&) = delete;
        VK_Renderer operator = (const VK_Renderer&) = delete;

        ErrorCode Startup();
        ErrorCode Shutdown();
        ErrorCode LateStartup();

        void Render();

        void WaitForFinish() const;
        void SetImGuiBackend(VK_ImGUI_Renderer* backend) { m_ImGuiBackend = backend; }

        NODISCARD inline VK_Device& GetDevice() const { return *m_Device; }
        NODISCARD inline VK_Instance& GetInstance() const { return *m_Instance; }
        NODISCARD inline VK_Swapchain& GetSwapchain() const { return *m_Swapchain; }
        NODISCARD inline VK_StagingManager& GetStagingManager() const { return *m_StagingManager; }
        NODISCARD inline VK_BindlessManager& GetBindlessManager() const { return *m_BindlessManager; }
    private:
        void OnRecreateSwapchain();

        NODISCARD ErrorCode SetupPipelines();

        void RecordGeometryPass(VkCommandBuffer cmdBuff);
        void RecordLightingPass(VkCommandBuffer cmdBuff, const AcquireImageInfo& swapchainImage);

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;

        std::unique_ptr<VK_StagingManager> m_StagingManager;
        std::unique_ptr<VK_BindlessManager> m_BindlessManager;
        std::unique_ptr<VK_GBuffer> m_GBuffer;

        std::vector<std::unique_ptr<VK_CmdBuffer>> m_GeometryPassCommandBuffers;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_LightingPassCommandBuffers;
        std::vector<std::unique_ptr<VK_Semaphore>> m_GeometryPassFinishedSemaphores;
        std::vector<std::unique_ptr<VK_Semaphore>> m_RenderFinishedSemaphores;
        std::vector<std::unique_ptr<VK_Fence>> m_InFlightFences;

        std::unique_ptr<VK_GraphicsPipeline> m_GeometryPassPipeline;
        std::unique_ptr<VK_GraphicsPipeline> m_LightingPassPipeline;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;
    };
}
