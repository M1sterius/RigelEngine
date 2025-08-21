#include "VK_GraphicsPipeline.hpp"
#include "VulkanUtility.hpp"
#include "Vertex.hpp"
#include "VK_DescriptorSet.hpp"
#include "ShaderStructs.hpp"
#include "VK_Fence.hpp"
#include "VK_ShaderModule.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GraphicsPipeline::VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo)
        : m_Device(device)
    {
        VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device.Get(), &layoutInfo, nullptr, &m_PipelineLayout), "Failed to create graphics pipeline layout!");

        pipelineInfo.layout = m_PipelineLayout;

        VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device.Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline), "Failed to create graphics pipeline!");
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

    std::unique_ptr<VK_GraphicsPipeline> VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(VK_Device& device,
        const VkFormat colorAttachmentFormat, const Shader::Variant& shaderVariant, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
    {
        const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
            shaderVariant.VertexModule->GetStageInfo(),
            shaderVariant.FragmentModule->GetStageInfo()
        };

        // Dynamic rendering attachments info
        auto renderingCreateInfo = MakeInfo<VkPipelineRenderingCreateInfo>();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorAttachmentFormat;
        renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
        renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        // Pipeline vertex input
        const auto bindingDescription = Vertex::GetBindingDescription();
        const auto attributeDescription = Vertex::GetAttributeDescriptions();

        auto vertexInputInfo = MakeInfo<VkPipelineVertexInputStateCreateInfo>();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescription.size();
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        // Input Assembly
        auto inputAssembly = MakeInfo<VkPipelineInputAssemblyStateCreateInfo>();
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport & Scissor
        auto viewportState = MakeInfo<VkPipelineViewportStateCreateInfo>();
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterizer
        auto rasterizer = MakeInfo<VkPipelineRasterizationStateCreateInfo>();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        // Multisampling
        auto multisampling = MakeInfo<VkPipelineMultisampleStateCreateInfo>();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Color Blend State
        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        // colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        // colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        // colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        // colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        // colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        // colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        auto colorBlending = MakeInfo<VkPipelineColorBlendStateCreateInfo>();
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // Depth and stencil
        auto depthStencil = MakeInfo<VkPipelineDepthStencilStateCreateInfo>();
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        constexpr VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
            VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
            VK_DYNAMIC_STATE_CULL_MODE
        };

        auto dynamicState = MakeInfo<VkPipelineDynamicStateCreateInfo>();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates));
        dynamicState.pDynamicStates = dynamicStates;

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        // Pipeline Layout (Add descriptors and push constants here)
        auto pipelineLayoutInfo = MakeInfo<VkPipelineLayoutCreateInfo>();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        auto pipelineInfo = MakeInfo<VkGraphicsPipelineCreateInfo>();
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = VK_NULL_HANDLE; // Layout is set inside the pipeline class constructor
        pipelineInfo.renderPass = VK_NULL_HANDLE;  // No traditional render pass, dynamic rendering used
        pipelineInfo.pNext = &renderingCreateInfo; // Attach dynamic rendering info

        return std::make_unique<VK_GraphicsPipeline>(device, pipelineLayoutInfo, pipelineInfo);
    }

    std::unique_ptr<VK_GraphicsPipeline> VK_GraphicsPipeline::CreateGeometryPassPipeline(VK_Device& device,
        const VkPipelineLayoutCreateInfo& pipelineLayoutInfo, const Shader::Variant& shader)
    {
        const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
            shader.VertexModule->GetStageInfo(),
            shader.FragmentModule->GetStageInfo()
        };

        constexpr VkFormat colorAttachmentFormats[] = {
            VK_FORMAT_R32G32B32A32_SFLOAT, // Position
            VK_FORMAT_R32G32B32A32_SFLOAT, // Normal
            VK_FORMAT_R32G32B32A32_SFLOAT // Albedo-spec
        };

        // Dynamic rendering attachments info
        auto renderingCreateInfo = MakeInfo<VkPipelineRenderingCreateInfo>();
        renderingCreateInfo.colorAttachmentCount = 3;
        renderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats;
        renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;

        // Pipeline vertex input
        const auto bindingDescription = Vertex::GetBindingDescription();
        const auto attributeDescription = Vertex::GetAttributeDescriptions();

        auto vertexInputInfo = MakeInfo<VkPipelineVertexInputStateCreateInfo>();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescription.size();
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        // Input Assembly
        auto inputAssembly = MakeInfo<VkPipelineInputAssemblyStateCreateInfo>();
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport & Scissor
        auto viewportState = MakeInfo<VkPipelineViewportStateCreateInfo>();
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterizer
        auto rasterizer = MakeInfo<VkPipelineRasterizationStateCreateInfo>();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        // Multisampling
        auto multisampling = MakeInfo<VkPipelineMultisampleStateCreateInfo>();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Blending
        std::array<VkPipelineColorBlendAttachmentState, 3> colorBlendAttachments {};

        colorBlendAttachments[0].blendEnable = VK_FALSE;
        colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachments[1].blendEnable = VK_FALSE;
        colorBlendAttachments[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachments[2].blendEnable = VK_FALSE;
        colorBlendAttachments[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        auto colorBlending = MakeInfo<VkPipelineColorBlendStateCreateInfo>();
        colorBlending.attachmentCount = colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();

        // Depth and stencil
        auto depthStencil = MakeInfo<VkPipelineDepthStencilStateCreateInfo>();
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        constexpr VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            // VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
            // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
            // VK_DYNAMIC_STATE_CULL_MODE
        };

        auto dynamicState = MakeInfo<VkPipelineDynamicStateCreateInfo>();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates));
        dynamicState.pDynamicStates = dynamicStates;

        auto pipelineInfo = MakeInfo<VkGraphicsPipelineCreateInfo>();
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = VK_NULL_HANDLE; // Layout is set inside the pipeline class constructor
        pipelineInfo.renderPass = VK_NULL_HANDLE;  // No traditional render pass, dynamic rendering used
        pipelineInfo.pNext = &renderingCreateInfo; // Attach dynamic rendering info

        return std::make_unique<VK_GraphicsPipeline>(device, pipelineLayoutInfo, pipelineInfo);
    }
}
