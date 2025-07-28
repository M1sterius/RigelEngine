#include "VK_Renderer.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"
#include "Shader.hpp"
#include "ShaderStructs.hpp"
#include "VK_BindlessManager.hpp"
#include "VulkanWrapper.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "AssetManager.hpp"
#include "BuiltInAssets.hpp"
#include "Engine.hpp"
#include "WindowManager.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "SubsystemGetters.hpp"

#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    VK_Renderer::VK_Renderer() = default;
    VK_Renderer::~VK_Renderer() = default;

    ErrorCode VK_Renderer::Startup()
    {
        Debug::Trace("Starting up Vulkan renderer backend.");

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), GetWindowManager()->GetWindowSize());
        m_BindlessManager = std::make_unique<VK_BindlessManager>(*this, *m_Device);

        CreateDepthBufferImage(GetWindowManager()->GetWindowSize());

        const auto framesInFlight = m_Swapchain->GetFramesInFlightCount();

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));
            m_RenderFinishedSemaphore.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device));
        }

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::LateStartup()
    {
        Debug::Trace("Vulkan renderer late startup.");
        ASSERT(m_ImGuiBackend, "ImGui backend was a nullptr");

        const auto shaderAsset = GetAssetManager()->Load<Shader>(BuiltInAssets::ShaderDefault);
        const auto defaultShader = shaderAsset->GetImpl();

        if (!defaultShader)
            return ErrorCode::RENDERER_LATE_STARTUP_FAILURE;

        const std::vector layouts = { m_BindlessManager->GetDescriptorSetLayout() };

        m_GraphicsPipeline = VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(*m_Device, m_Swapchain->GetSwapchainImageFormat(), defaultShader, layouts);

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::Shutdown()
    {
        // this shutdown method is called inside Renderer::Shutdown()!
        m_Device->WaitIdle();

        Debug::Trace("Shutting down Vulkan renderer.");

        return ErrorCode::OK;
    }

    void VK_Renderer::RecreateSwapchain()
    {
        GetWindowManager()->WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = GetWindowManager()->GetWindowSize();
        const auto vsync = GetWindowManager()->IsVsyncEnabled();

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

    void VK_Renderer::RenderScene(VkCommandBuffer cmdBuffer)
    {
        const auto& renderInfo = GetRenderer()->GetSceneRenderInfo();

        if (!renderInfo.CameraPresent)
            return;

        for (uint32_t i = 0; i < renderInfo.Models.size(); ++i)
        {
            const auto& model = renderInfo.Models[i];
            const auto& modelTransform = renderInfo.Transforms[i];

            const auto vertexBuffer = model->GetVertexBuffer();
            const auto indexBuffer = model->GetIndexBuffer();

            const VkBuffer pVertexBuffers[] = {vertexBuffer->GetMemoryBuffer().Get()};
            const VkDeviceSize pOffsets[] = {0};

            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, pVertexBuffers, pOffsets);
            vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->GetMemoryBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

            int32_t vertexOffset = 0;
            for (auto node = model->GetNodeIterator(); node.Valid(); ++node)
            {
                const auto mvp = renderInfo.ProjView * modelTransform * node->Transform;

                for (const auto& mesh : node->Meshes)
                {
                    auto pc = PushConstantData{
                        .MVP = mvp,
                        .MeshIndex = mesh.MaterialIndex
                    };

                    vkCmdPushConstants(
                        cmdBuffer,
                        m_GraphicsPipeline->GetLayout(),
                        VK_SHADER_STAGE_VERTEX_BIT,
                        0,
                        sizeof(pc),
                        &pc
                    );

                    vkCmdDrawIndexed(
                        cmdBuffer,
                        mesh.IndexCount,
                        1,
                        mesh.FirstIndex,
                        vertexOffset,
                        0
                    );

                    vertexOffset = mesh.FirstVertex + mesh.VertexCount;
                }
            }
        }
    }

    void VK_Renderer::RecordCommandBuffer(const VK_CmdBuffer& commandBuffer, const AcquireImageInfo& image)
    {
        commandBuffer.BeginRecording(0);
        const auto vkCmdBuffer = commandBuffer.Get();

        VK_Image::CmdTransitionLayout(vkCmdBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(),
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

        vkCmdBeginRendering(vkCmdBuffer, &renderingInfo);
        vkCmdBindPipeline(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Get());

        const VkDescriptorSet sets[] = { m_BindlessManager->GetDescriptorSet() };
        vkCmdBindDescriptorSets(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetLayout(), 0, 1, sets, 0, nullptr);

        const auto viewportSize = glm::vec2(static_cast<float>(m_Swapchain->GetExtent().width), static_cast<float>(m_Swapchain->GetExtent().height));
        VK_CmdBuffer::CmdSetViewport(vkCmdBuffer, {0.0, 0.0}, viewportSize, {0.0, 1.0});
        VK_CmdBuffer::CmdSetScissor(vkCmdBuffer, {0.0, 0.0}, m_Swapchain->GetExtent());

        RenderScene(vkCmdBuffer);

        vkCmdEndRendering(vkCmdBuffer);

        auto uiColorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        uiColorAttachment.imageView = image.imageView;
        uiColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        uiColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        uiColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto uiRenderingInfo = MakeInfo<VkRenderingInfo>();
        uiRenderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        uiRenderingInfo.layerCount = 1;
        uiRenderingInfo.colorAttachmentCount = 1;
        uiRenderingInfo.pColorAttachments = &uiColorAttachment;

        vkCmdBeginRendering(vkCmdBuffer, &uiRenderingInfo);
        m_ImGuiBackend->RenderFrame(commandBuffer.Get());
        vkCmdEndRendering(vkCmdBuffer);

        VK_Image::CmdTransitionLayout(vkCmdBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(),
            VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        commandBuffer.EndRecording();
    }

    void VK_Renderer::Render()
    {
        if (GetWindowManager()->GetWindowResizeFlag())
        {
            GetWindowManager()->ResetWindowResizeFlag();
            RecreateSwapchain();
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        m_InFlightFences[frameIndex]->Wait();

        m_BindlessManager->UpdateStorageBuffer();
        const auto image = m_Swapchain->AcquireNextImage();

        const auto& commandBuffer = *m_CommandBuffers[frameIndex];
        commandBuffer.Reset(0);

        RecordCommandBuffer(commandBuffer, image);

        const VkCommandBuffer submitBuffers[] = { commandBuffer.Get() };
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

        m_Device->SubmitGraphicsQueue(1, &submitInfo, fence->Get());

        m_Swapchain->Present(image.imageIndex, signalSemaphores[0]);
    }

    void VK_Renderer::WaitForFinish() const
    {
        m_Device->WaitIdle();
    }

}
