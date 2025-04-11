#include "VK_Renderer.hpp"

#include "Shader.hpp"
#include "VulkanWrapper.hpp"
#include "VK_Config.hpp"
#include "VK_Model.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "AssetManager.hpp"
#include "VK_Shader.hpp"
#include "Engine.hpp"
#include "WindowManager.hpp"
#include "GameObject.hpp"
#include "Model.hpp"
#include "Transform.hpp"

#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    VK_Renderer::VK_Renderer() :
    m_WindowManager(Engine::Get().GetWindowManager()),
    m_AssetManager(Engine::Get().GetAssetManager())
    { Startup(); }
    VK_Renderer::~VK_Renderer() { Shutdown(); }

    void VK_Renderer::Startup()
    {
        Debug::Trace("Starting up Vulkan renderer.");

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), m_WindowManager.GetSize());
        CreateDepthBufferImage(m_WindowManager.GetSize());

        const auto framesInFlight = m_Swapchain->GetFramesInFlightCount();

        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = framesInFlight;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(*m_Device, poolSizes, framesInFlight);

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));
            m_RenderFinishedSemaphore.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device));

            m_UniformBuffers.emplace_back(std::make_unique<VK_UniformBuffer>(*m_Device, sizeof(DefaultUBO)));

            auto setBuilder = VK_DescriptorSetBuilder(*m_Device);
            setBuilder.AddUniformBuffer(*m_UniformBuffers[i], 0);

            m_DescriptorSets.emplace_back(std::make_unique<VK_DescriptorSet>(*m_Device, *m_DescriptorPool, setBuilder));
        }

        m_Initialized = true;
    }

    void VK_Renderer::InitImGUI()
    {

    }

    void VK_Renderer::LateInit()
    {
        // We do this to give info about descriptor layout to the rendering pipeline
        auto layoutBuilder = VK_DescriptorSetBuilder(*m_Device);
        layoutBuilder.AddUniformBuffer(*m_UniformBuffers.back(), 0);
        const auto layout = layoutBuilder.BuildLayout();

        const auto& defaultShader = m_AssetManager.Load<Shader>("Assets/EngineAssets/Shaders/DefaultShader.spv")->GetBackend<VK_Shader>();
        m_GraphicsPipeline = VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(*m_Device, m_Swapchain->GetSwapchainImageFormat(), defaultShader, layout);

        // After the pipeline is created, the descriptor layout is no longer needed
        vkDestroyDescriptorSetLayout(m_Device->Get(), layout, nullptr);
    }

    void VK_Renderer::Shutdown()
    {
        m_Device->WaitIdle();

        Debug::Trace("Shutting down Vulkan renderer.");
    }

    void VK_Renderer::RecreateSwapchain()
    {
        m_WindowManager.WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = m_WindowManager.GetSize();
        const auto vsync = m_WindowManager.IsVsyncEnabled();

        CreateDepthBufferImage(windowSize);
        m_Swapchain->SetupSwapchain(windowSize, vsync);
    }

    void VK_Renderer::CreateDepthBufferImage(const glm::uvec2 size)
    {
        if (m_DepthBufferImage) m_DepthBufferImage.reset();

        m_DepthBufferImage = std::make_unique<VK_Image>(*m_Device, size, VK_FORMAT_D32_SFLOAT,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_Image::TransitionLayout(*m_Device, *m_DepthBufferImage, m_DepthBufferImage->GetFormat(),
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    }

    void VK_Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, const AcquireImageInfo& image, SceneRenderInfo& sceneRenderInfo) const
    {
        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();

        VK_Image::CmdTransitionLayout(commandBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(),
            VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        auto colorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        colorAttachment.imageView = image.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.2f, 0.3f, 0.4f, 1.0f}};

        auto depthAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        depthAttachment.imageView = m_DepthBufferImage->GetView();
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};

        auto renderingInfo = MakeInfo<VkRenderingInfo>();
        renderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Get());

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_Swapchain->GetExtent().width);
        viewport.height = static_cast<float>(m_Swapchain->GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = m_Swapchain->GetExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        auto& camera = sceneRenderInfo.MainCamera;
        const auto projView = camera->GetProjection() * camera->GetView();

        for (const auto& model : sceneRenderInfo.Models)
        {
            const auto mvp = projView * model->GetGameObject()->GetTransform()->GetModel();

            vkCmdPushConstants(
                commandBuffer,
                m_GraphicsPipeline->GetLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(glm::mat4),
                &mvp
            );

            const auto& vkModel = model->GetModel()->GetBackend<VK_Model>();

            const VkBuffer vertexBuffers[] = {vkModel.GetVertexBuffer().GetMemoryBuffer().Get()};
            constexpr VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffer, vkModel.GetIndexBuffer().GetMemoryBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, vkModel.GetIndexBuffer().GetIndexCount(), 1, 0, 0, 0);
        }

        vkCmdEndRendering(commandBuffer);

        VK_Image::CmdTransitionLayout(commandBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(),
            VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    void VK_Renderer::Render()
    {
        if (m_WindowManager.GetWindowResizeFlag())
        {
            m_WindowManager.ResetWindowResizeFlag();
            RecreateSwapchain();
            return;
        }

        auto& sceneRenderInfo = Engine::Get().GetRenderer().GetSceneRenderInfo();

        if (sceneRenderInfo.MainCamera.IsNull())
        {
            Debug::Error("No camera on the active scene. Scene rendering aborted!");
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        m_InFlightFences[frameIndex]->Wait();

        const auto image = m_Swapchain->AcquireNextImage();

        const auto& commandBuffer = m_CommandBuffers[frameIndex];
        commandBuffer->Reset(0);

        commandBuffer->BeginRecording(0);
        RecordCommandBuffer(commandBuffer->Get(), image, sceneRenderInfo);
        commandBuffer->EndRecording();

        const VkCommandBuffer submitBuffers[] = { commandBuffer->Get() };
        const VkSemaphore waitSemaphores[] = { image.availableSemaphore };
        const VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore[frameIndex]->Get() };

        constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        auto submitInfo = MakeInfo<VkSubmitInfo>();
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitBuffers;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        const auto& fence = m_InFlightFences[frameIndex];
        fence->Reset();

        vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, fence->Get());

        m_Swapchain->Present(image.imageIndex, signalSemaphores[0]);
    }

    void VK_Renderer::WaitForFinish() const
    {
        m_Device->WaitIdle();
    }

}
