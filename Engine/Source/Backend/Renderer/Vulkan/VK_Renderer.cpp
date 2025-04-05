#include "VK_Renderer.hpp"
#include "VulkanWrapper.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "AssetManager.hpp"
#include "Shader.hpp"
#include "Engine.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"

namespace rge::backend
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
        auto layoutBuilder = VK_DescriptorSetBuilder(*m_Device);
        layoutBuilder.AddUniformBuffer(*m_UniformBuffers.back(), 0);
        const auto layout = layoutBuilder.BuildLayout();

        const auto& defaultShader = m_AssetManager.Load<Shader>("Assets/EngineAssets/Shaders/DefaultShader.spv")->GetBackendShader<VK_Shader>();
        m_GraphicsPipeline = VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(*m_Device, m_Swapchain->GetSwapchainImageFormat(), defaultShader, layout);

        vkDestroyDescriptorSetLayout(m_Device->Get(), layout, nullptr);

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 1.0}, {1.0f, 0.0f}},
            {{0.5f, -0.5f, 1.0}, {0.0f, 1.0f}},
            {{0.5f, 0.5f, 1.0}, {0.0f, 0.0f}},
            {{-0.5f, 0.5f, 1.0}, {0.0f, 0.0f}}
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 3, 0
        };

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(*m_Device, vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(*m_Device, indices);
    }

    void VK_Renderer::Shutdown()
    {
        m_Device->WaitIdle();

        Debug::Trace("Shutting down Vulkan renderer.");
    }

    void VK_Renderer::RecreateSwapchain() const
    {
        m_WindowManager.WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = m_WindowManager.GetSize();
        const auto vsync = m_WindowManager.IsVsyncEnabled();

        m_Swapchain->SetupSwapchain(windowSize, vsync);
    }

    void VK_Renderer::UpdateUniformBuffer(VK_UniformBuffer& buffer)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        DefaultUBO ubo {};
        auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto proj = glm::perspective(glm::radians(45.0f), (float)m_Swapchain->GetExtent().width / (float)m_Swapchain->GetExtent().height, 0.1f, 10.0f);
        proj[1][1] *= -1;

        ubo.MVP = proj * view * model;

        buffer.UploadData(0, sizeof(ubo), &ubo);
    }

    void VK_Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, const AcquireImageInfo& image) const
    {
        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();

        VK_Image::CmdTransitionLayout(commandBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo colorAttachment {};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = image.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.2f, 0.3f, 0.4f, 1.0f}};

        VkRenderingInfo renderingInfo {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

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

        const VkBuffer vertexBuffers[] = {m_VertexBuffer->GetMemoryBuffer().Get()};
        constexpr VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetMemoryBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

        const auto currentDescriptor = m_DescriptorSets[frameIndex]->Get();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetLayout(), 0, 1, &currentDescriptor, 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, m_IndexBuffer->GetIndexCount(), 1, 0, 0, 0);

        vkCmdEndRendering(commandBuffer);
        VK_Image::CmdTransitionLayout(commandBuffer, image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    void VK_Renderer::Render()
    {
        if (m_WindowManager.GetWindowResizeFlag())
        {
            m_WindowManager.ResetWindowResizeFlag();
            RecreateSwapchain();
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        m_InFlightFences[frameIndex]->Wait();

        const auto image = m_Swapchain->AcquireNextImage();

        const auto& commandBuffer = m_CommandBuffers[frameIndex];
        commandBuffer->Reset(0);

        const auto& uniformBuffer = m_UniformBuffers[frameIndex];
        UpdateUniformBuffer(*uniformBuffer);

        commandBuffer->BeginRecording(0);
        RecordCommandBuffer(commandBuffer->Get(), image);
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
}
