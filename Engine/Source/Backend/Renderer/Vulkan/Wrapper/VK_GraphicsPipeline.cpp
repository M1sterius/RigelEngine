#include "VK_GraphicsPipeline.hpp"
#include "VK_ShaderModule.hpp"
#include "VK_DescriptorSet.hpp"
#include "../VK_GBuffer.hpp"
#include "../Helpers/VulkanUtility.hpp"
#include "../Helpers/Vertex.hpp"

namespace Rigel::Backend::Vulkan
{
    GraphicsPipelineConfigInfo GraphicsPipelineConfigInfo::Make()
    {
        auto configInfo = GraphicsPipelineConfigInfo();

        // Input assembly
        configInfo.InputAssembly = MakeInfo<VkPipelineInputAssemblyStateCreateInfo>();
        configInfo.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.InputAssembly.primitiveRestartEnable = VK_FALSE;

        // Rasterizer
        configInfo.Rasterizer = MakeInfo<VkPipelineRasterizationStateCreateInfo>();
        configInfo.Rasterizer.depthClampEnable = VK_FALSE;
        configInfo.Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        configInfo.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.Rasterizer.lineWidth = 1.0f;
        configInfo.Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        configInfo.Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        configInfo.Rasterizer.depthBiasEnable = VK_FALSE;

        // Multisampling
        configInfo.Multisampling = MakeInfo<VkPipelineMultisampleStateCreateInfo>();
        configInfo.Multisampling.sampleShadingEnable = VK_FALSE;
        configInfo.Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Depth and stencil
        configInfo.DepthStencil = MakeInfo<VkPipelineDepthStencilStateCreateInfo>();
        configInfo.DepthStencil.depthTestEnable = VK_TRUE;
        configInfo.DepthStencil.depthWriteEnable = VK_TRUE;
        configInfo.DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.DepthStencil.depthBoundsTestEnable = VK_FALSE;
        configInfo.DepthStencil.stencilTestEnable = VK_FALSE;

        // Always-enabled dynamic states
        configInfo.DynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        configInfo.NoVertexInput = false;

        return configInfo;
    }

    VK_GraphicsPipeline::VK_GraphicsPipeline(VK_Device& device, const GraphicsPipelineConfigInfo& configInfo,
                                             VkPipelineLayout pipelineLayout)
            : m_Device(device), m_PipelineLayout(pipelineLayout)
    {
        // Dynamic rendering info
        auto renderingCreateInfo = MakeInfo<VkPipelineRenderingCreateInfo>();
        renderingCreateInfo.colorAttachmentCount = configInfo.ColorAttachmentFormats.size();
        renderingCreateInfo.pColorAttachmentFormats = configInfo.ColorAttachmentFormats.data();
        renderingCreateInfo.depthAttachmentFormat = configInfo.DepthAttachmentFormat;
        renderingCreateInfo.stencilAttachmentFormat = configInfo.StencilAttachmentFormat;

        // Vertex input info
        auto vertexInputInfo = MakeInfo<VkPipelineVertexInputStateCreateInfo>();
        if (configInfo.NoVertexInput)
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        }
        else
        {
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &configInfo.VertexBindingDescription;
            vertexInputInfo.vertexAttributeDescriptionCount = configInfo.VertexAttributeDescriptions.size();
            vertexInputInfo.pVertexAttributeDescriptions = configInfo.VertexAttributeDescriptions.data();
        }

        // Viewport and scissor. They are always dynamic!
        auto viewportState = MakeInfo<VkPipelineViewportStateCreateInfo>();
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Color blending
        auto colorBlending = MakeInfo<VkPipelineColorBlendStateCreateInfo>();
        colorBlending.attachmentCount = configInfo.BlendAttachments.size();
        colorBlending.pAttachments = configInfo.BlendAttachments.data();

        // Dynamic states
        auto dynamicState = MakeInfo<VkPipelineDynamicStateCreateInfo>();
        dynamicState.dynamicStateCount = configInfo.DynamicStates.size();
        dynamicState.pDynamicStates = configInfo.DynamicStates.data();

        auto pipelineInfo = MakeInfo<VkGraphicsPipelineCreateInfo>();
        pipelineInfo.stageCount = configInfo.ShaderStages.size();
        pipelineInfo.pStages = configInfo.ShaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.InputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &configInfo.Rasterizer;
        pipelineInfo.pMultisampleState = &configInfo.Multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &configInfo.DepthStencil;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.pNext = &renderingCreateInfo;

        VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device.Get(), VK_NULL_HANDLE,
            1, &pipelineInfo, nullptr, &m_GraphicsPipeline), "Failed to create vulkan graphics pipeline!");
    }

    VK_GraphicsPipeline::~VK_GraphicsPipeline()
    {
        vkDestroyPipelineLayout(m_Device.Get(), m_PipelineLayout, nullptr);
        vkDestroyPipeline(m_Device.Get(), m_GraphicsPipeline, nullptr);
    }

    void VK_GraphicsPipeline::CmdBind(VkCommandBuffer commandBuffer) const
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    }

    void VK_GraphicsPipeline::CmdSetViewport(VkCommandBuffer commandBuffer, const glm::vec2 pos,
        const glm::uvec2 size) const
    {
        const VkViewport viewport {
            .x = pos.x,
            .y = pos.y,
            .width = static_cast<float>(size.x),
            .height = static_cast<float>(size.y),
            .minDepth = 0,
            .maxDepth = 1
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    void VK_GraphicsPipeline::CmdSetScissor(VkCommandBuffer commandBuffer, const glm::ivec2 offset,
        const VkExtent2D extent) const
    {
        const VkRect2D scissor {
            .offset = {offset.x, offset.y},
            .extent = extent
        };

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VK_GraphicsPipeline::CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const
    {
        vkCmdSetDepthTestEnable(commandBuffer, enabled);
    }

    void VK_GraphicsPipeline::CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const
    {
        vkCmdSetDepthWriteEnable(commandBuffer, enabled);
    }

    void VK_GraphicsPipeline::CmdSetCullMode(VkCommandBuffer commandBuffer, const VkCullModeFlags mode) const
    {
        vkCmdSetCullMode(commandBuffer, mode);
    }

    VkPipelineLayout VK_GraphicsPipeline::CreateLayout(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutCreateInfo)
    {
        VkPipelineLayout pipelineLayout;
        VK_CHECK_RESULT(vkCreatePipelineLayout(device.Get(), &layoutCreateInfo, nullptr, &pipelineLayout), "Failed to create vulkan pipeline layout!");

        return pipelineLayout;
    }
}
