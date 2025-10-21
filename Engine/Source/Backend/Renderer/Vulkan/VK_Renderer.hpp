#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>

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
    class VK_StagingManager;

    class VK_GBuffer;
    class VK_GPUScene;
    class VK_GeometryPass;
    class VK_LightingPass;
    class VK_ForwardPass;

    class VK_ImGUI_Renderer;
    class VK_BindlessManager;

    struct AcquireImageInfo;
    struct MeshData;

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

        NODISCARD VK_Device& GetDevice() const { return *m_Device; }
        NODISCARD VK_Instance& GetInstance() const { return *m_Instance; }
        NODISCARD VK_Swapchain& GetSwapchain() const { return *m_Swapchain; }
        NODISCARD VK_StagingManager& GetStagingManager() const { return *m_StagingManager; }
        NODISCARD VK_BindlessManager& GetBindlessManager() const { return *m_BindlessManager; }
    private:
        void OnWindowResize();

        std::unique_ptr<VK_Instance> m_Instance;
        std::unique_ptr<VK_Surface> m_Surface;
        std::unique_ptr<VK_Device> m_Device;
        std::unique_ptr<VK_Swapchain> m_Swapchain;

        std::unique_ptr<VK_StagingManager> m_StagingManager;
        std::unique_ptr<VK_BindlessManager> m_BindlessManager;

        std::unique_ptr<VK_GBuffer> m_GBuffer;
        std::unique_ptr<VK_GPUScene> m_GPUScene;
        std::unique_ptr<VK_GeometryPass> m_GeometryPass;
        std::unique_ptr<VK_LightingPass> m_LightingPass;
        std::unique_ptr<VK_ForwardPass> m_ForwardPass;

        std::vector<std::unique_ptr<VK_Semaphore>> m_GeometryPassFinishedSemaphores;
        std::vector<std::unique_ptr<VK_Semaphore>> m_LightingPassFinishedSemaphores;
        std::vector<std::unique_ptr<VK_Semaphore>> m_ForwardPassFinishedSemaphores;
        std::vector<std::unique_ptr<VK_Fence>> m_InFlightFences;

        VK_ImGUI_Renderer* m_ImGuiBackend = nullptr;
    };
}
