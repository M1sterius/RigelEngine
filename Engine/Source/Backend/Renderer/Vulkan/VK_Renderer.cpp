#include "VK_Renderer.hpp"
#include "Assets/Shader.hpp"
#include "Assets/Model.hpp"
#include "VK_BindlessManager.hpp"
#include "VK_GPUScene.hpp"
#include "VK_GBuffer.hpp"
#include "VK_StagingManager.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VulkanWrapper.hpp"
#include "RenderPasses/VK_GeometryPass.hpp"
#include "RenderPasses/VK_LightingPass.hpp"
#include "Helpers/MakeInfo.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"
#include "Subsystems/Time.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/AssetManager/BuiltInAssets.hpp"
#include "Subsystems/WindowManager/WindowManager.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/SubsystemGetters.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    VK_Renderer::VK_Renderer() = default;
    VK_Renderer::~VK_Renderer() = default;

    ErrorCode VK_Renderer::Startup()
    {
        Debug::Trace("Starting up vulkan rendering backend.");

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), GetWindowManager()->GetWindowSize());
        m_BindlessManager = std::make_unique<VK_BindlessManager>(*this, *m_Device);
        m_StagingManager = std::make_unique<VK_StagingManager>(*m_Device);

        m_GBuffer = std::make_unique<VK_GBuffer>(*m_Device, GetWindowManager()->GetWindowSize());
        m_GPUScene = std::make_unique<VK_GPUScene>(*m_Device, *m_Swapchain);
        m_GeometryPass = std::make_unique<VK_GeometryPass>(*m_Device, *m_Swapchain, *m_BindlessManager, *m_GBuffer);
        m_LightingPass = std::make_unique<VK_LightingPass>(*m_Device, *m_Swapchain, *m_GBuffer);

        for (uint32_t i = 0; i < m_Swapchain->GetFramesInFlightCount(); ++i)
        {
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));
            m_GeometryPassFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
            m_LightingPassFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
        }

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::LateStartup()
    {
        Debug::Trace("Vulkan renderer late startup.");
        ASSERT(m_ImGuiBackend, "ImGui backend was a nullptr");

        m_LightingPass->SetImGuiBackend(m_ImGuiBackend);

        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureError, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureBlack, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureWhite, true);

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::Shutdown()
    {
        // this shutdown method is called inside Renderer::Shutdown()!
        m_Device->WaitIdle();

        Debug::Trace("Shutting down vulkan rendering backend.");

        return ErrorCode::OK;
    }
    
    void VK_Renderer::OnWindowResize()
    {
        GetWindowManager()->WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = GetWindowManager()->GetWindowSize();
        const auto vsync = GetWindowManager()->IsVsyncEnabled();

        m_Swapchain->Recreate(windowSize, vsync);
        m_GBuffer->Recreate(windowSize);
        m_LightingPass->Recreate();
    }

    void VK_Renderer::Render()
    {
        // Swapchain resize
        if (GetWindowManager()->GetWindowResizeFlag())
        {
            GetWindowManager()->ResetWindowResizeFlag();
            OnWindowResize();
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        const auto& fence = m_InFlightFences[frameIndex];

        fence->Wait();
        fence->Reset();

        const auto swapchainImage = m_Swapchain->AcquireNextImage();
        const auto sceneRenderInfo = GetRenderer()->GetSceneRenderInfo();

        m_GeometryPass->SetMeshData(sceneRenderInfo, frameIndex);
        const auto geometryPassCommandBuffer = m_GeometryPass->RecordCommandBuffer(frameIndex);

        m_LightingPass->SetLightData(sceneRenderInfo, frameIndex);
        const auto lightingPassCommandBuffer = m_LightingPass->RecordCommandBuffer(swapchainImage, frameIndex);

        const VkCommandBuffer geometryPassSubmitBuffers[] = {geometryPassCommandBuffer};
        const VkSemaphore geometryPassSignalSemaphores[] = {m_GeometryPassFinishedSemaphores[frameIndex]->Get()};

        auto geometryPassSubmitInfo = MakeInfo<VkSubmitInfo>();
        geometryPassSubmitInfo.pWaitDstStageMask = nullptr;
        geometryPassSubmitInfo.commandBufferCount = 1;
        geometryPassSubmitInfo.pCommandBuffers = geometryPassSubmitBuffers;
        geometryPassSubmitInfo.waitSemaphoreCount = 0;
        geometryPassSubmitInfo.signalSemaphoreCount = 1;
        geometryPassSubmitInfo.pSignalSemaphores = geometryPassSignalSemaphores;

        m_Device->SubmitToQueue(QueueType::Graphics, 1, &geometryPassSubmitInfo, nullptr);

        const VkCommandBuffer lightingPassSubmitBuffers[] = {lightingPassCommandBuffer};
        const VkSemaphore lightingPassWaitSemaphores[] = {
            m_GeometryPassFinishedSemaphores[frameIndex]->Get(),
            swapchainImage.availableSemaphore
        };
        const VkSemaphore lightingPassSignalSemaphores[] = {m_LightingPassFinishedSemaphores[frameIndex]->Get()};

        constexpr VkPipelineStageFlags lightingPassWaitStages[] = {
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        auto lightingPassSubmitInfo = MakeInfo<VkSubmitInfo>();

        lightingPassSubmitInfo.pWaitDstStageMask = lightingPassWaitStages;
        lightingPassSubmitInfo.commandBufferCount = 1;
        lightingPassSubmitInfo.pCommandBuffers = lightingPassSubmitBuffers;
        lightingPassSubmitInfo.waitSemaphoreCount = 2;
        lightingPassSubmitInfo.pWaitSemaphores = lightingPassWaitSemaphores;
        lightingPassSubmitInfo.signalSemaphoreCount = 1;
        lightingPassSubmitInfo.pSignalSemaphores = lightingPassSignalSemaphores;

        m_Device->SubmitToQueue(QueueType::Graphics, 1, &lightingPassSubmitInfo, fence->Get());

        const auto presentWaitSemaphore = m_LightingPassFinishedSemaphores[frameIndex]->Get();

        m_Swapchain->Present(swapchainImage.imageIndex, presentWaitSemaphore);
    }

    void VK_Renderer::WaitForFinish() const
    {
        m_Device->WaitIdle();
    }
}
