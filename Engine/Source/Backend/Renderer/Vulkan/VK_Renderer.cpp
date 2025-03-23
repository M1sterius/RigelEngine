#include "VK_Renderer.hpp"

#include "VulkanException.hpp"
#include "VK_Shader.hpp"
#include "VK_CmdBuffer.hpp"
#include "VK_Fence.hpp"
#include "VK_Semaphore.hpp"
#include "VK_Config.hpp"
#include "VK_Instance.hpp"
#include "VK_Surface.hpp"
#include "VK_Device.hpp"
#include "VK_Swapchain.hpp"
#include "VK_Image.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "Time.hpp"

#include "Engine.hpp"
#include "WindowManager.hpp"

#include "vulkan.h"

namespace rge::backend
{
    VK_Renderer::VK_Renderer() : m_WindowManager(Engine::Get().GetWindowManager())
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

        for (uint32_t i = 0; i < framesInFlight; i++)
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));

        for (uint32_t i = 0; i < framesInFlight; i++)
            m_RenderFinishedSemaphore.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));

        for (uint32_t i = 0; i < framesInFlight; i++)
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device));

        m_Initialized = true;
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

    std::pair<VkPipeline, VkPipelineLayout> VK_Renderer::CreateGraphicsPipeline() const
    {
        // Temporary method that creates graphics pipeline for dynamic rendering
        // TODO: Later should be properly abstracted into a class

        VkPipeline graphicsPipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        const auto shader = std::make_unique<VK_Shader>("Assets/EngineAssets/Shaders/Test.vert.spv", "Assets/EngineAssets/Shaders/Test.frag.spv");
        const auto shaderStagesInfo = shader->GetShaderStagesInfo();

        const auto swapchainColorFormat = m_Swapchain->GetSwapchainImageFormat();

        VkPipelineRenderingCreateInfo renderingCreateInfo {};
        renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &swapchainColorFormat;
        renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        // 4. Define pipeline vertex input (assuming no vertex buffer)
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        // 5. Input Assembly (triangle list)
        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // 6. Viewport & Scissor (Dynamic states)
        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // 7. Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        // 8. Multisampling (No MSAA)
        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // 9. Color Blend State
        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates));
        dynamicState.pDynamicStates = dynamicStates;

        // 11. Pipeline Layout (No descriptors here, but you can add them)
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        if (const auto result = vkCreatePipelineLayout(m_Device->Get(), &pipelineLayoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS)
            throw VulkanException("Failed to create graphics pipeline layout!", result);

        // 12. Final Graphics Pipeline Create Info
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStagesInfo.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;  // No traditional render pass
        pipelineInfo.pNext = &renderingCreateInfo; // Attach dynamic rendering info

        if (const auto result = vkCreateGraphicsPipelines(m_Device->Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline); result != VK_SUCCESS)
            throw VulkanException("Failed to create graphics pipeline!", result);

        return {graphicsPipeline, pipelineLayout};
    }

    void VK_Renderer::InitImGUI()
    {

    }

    void VK_Renderer::Render()
    {
        static int _tm = 1;
        if (_tm == 1) // TODO: Rework this fucking abomination ASAP!!!
        {
            auto pipeline = CreateGraphicsPipeline();
            m_GraphicsPipeline = pipeline.first;
            _tm++;
        }

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
        commandBuffer->BeginRecording(0);

        VK_Image::CmdTransitionLayout(commandBuffer->Get(), image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo colorAttachment {};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = image.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.0f, 0.5f, 1.0f, 1.0f}};

        VkRenderingInfo renderingInfo {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(commandBuffer->Get(), &renderingInfo);

        vkCmdBindPipeline(commandBuffer->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_Swapchain->GetExtent().width);
        viewport.height = static_cast<float>(m_Swapchain->GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer->Get(), 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = m_Swapchain->GetExtent();
        vkCmdSetScissor(commandBuffer->Get(), 0, 1, &scissor);

        vkCmdDraw(commandBuffer->Get(), 3, 1, 0, 0);

        vkCmdEndRendering(commandBuffer->Get());

        VK_Image::CmdTransitionLayout(commandBuffer->Get(), image.image, m_Swapchain->GetSwapchainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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
