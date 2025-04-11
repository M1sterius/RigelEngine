#include "VK_GraphicsPipeline.hpp"
#include "VK_Device.hpp"
#include "VK_Shader.hpp"
#include "VulkanException.hpp"
#include "VK_VertexBuffer.hpp"
#include "VK_DescriptorSet.hpp"
#include "MakeInfo.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GraphicsPipeline::VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo)
        : m_Device(device)
    {
        if (const auto result = vkCreatePipelineLayout(m_Device.Get(), &layoutInfo, nullptr, &m_PipelineLayout); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan graphics pipeline layout!", result);

        pipelineInfo.layout = m_PipelineLayout;

        if (const auto result = vkCreateGraphicsPipelines(m_Device.Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline); result != VK_SUCCESS)
            throw VulkanException("Failed to create graphics pipeline!", result);
    }

    VK_GraphicsPipeline::~VK_GraphicsPipeline()
    {
        vkDestroyPipelineLayout(m_Device.Get(), m_PipelineLayout, nullptr);
        vkDestroyPipeline(m_Device.Get(), m_GraphicsPipeline, nullptr);
    }

    std::unique_ptr<VK_GraphicsPipeline> VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(VK_Device& device,
        const VkFormat swapchainImageFormat, const VK_Shader& shader, VkDescriptorSetLayout descriptorSetLayout)
    {
        const auto shaderStagesInfo = shader.GetShaderStagesInfo();

        // Dynamic rendering attachments info
        auto renderingCreateInfo = MakeInfo<VkPipelineRenderingCreateInfo>();
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;
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

        constexpr VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        auto dynamicState = MakeInfo<VkPipelineDynamicStateCreateInfo>();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates));
        dynamicState.pDynamicStates = dynamicStates;

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(glm::mat4);

        // Pipeline Layout (Add descriptors and push constants here)
        auto pipelineLayoutInfo = MakeInfo<VkPipelineLayoutCreateInfo>();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = & pushConstantRange;
        // pipelineLayoutInfo.setLayoutCount = 1;
        // pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        auto pipelineInfo = MakeInfo<VkGraphicsPipelineCreateInfo>();
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStagesInfo.data();
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
