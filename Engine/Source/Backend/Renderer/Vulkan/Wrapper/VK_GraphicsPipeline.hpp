#pragma once

#include "Core.hpp"
#include "Assets/Shader.hpp"
#include "../Helpers/MakeInfo.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Shader;

    struct GraphicsPipelineConfigInfo
    {
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;

        std::vector<VkFormat> ColorAttachmentFormats;
        VkFormat DepthAttachmentFormat;
        VkFormat StencilAttachmentFormat;

        bool NoVertexInput;
        VkVertexInputBindingDescription VertexBindingDescription;
        std::vector<VkVertexInputAttributeDescription> VertexAttributeDescriptions;

        VkPipelineInputAssemblyStateCreateInfo InputAssembly;

        VkPipelineRasterizationStateCreateInfo Rasterizer;

        VkPipelineMultisampleStateCreateInfo Multisampling;

        std::vector<VkPipelineColorBlendAttachmentState> BlendAttachments;

        VkPipelineDepthStencilStateCreateInfo DepthStencil;

        std::vector<VkDynamicState> DynamicStates;

        NODISCARD static GraphicsPipelineConfigInfo Make()
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
    private:
        GraphicsPipelineConfigInfo() = default;
    };

    class VK_GraphicsPipeline 
    {
    public:
        VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo);
        VK_GraphicsPipeline(VK_Device& device, const GraphicsPipelineConfigInfo& configInfo, VkPipelineLayout pipelineLayout);
        ~VK_GraphicsPipeline();

        VK_GraphicsPipeline(const VK_GraphicsPipeline&) = delete;
        VK_GraphicsPipeline operator = (const VK_GraphicsPipeline&) = delete;

        NODISCARD VkPipeline Get() const { return m_GraphicsPipeline; }
        NODISCARD VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

        void CmdBind(VkCommandBuffer commandBuffer) const;
        void CmdSetViewport(VkCommandBuffer commandBuffer, const glm::vec2 pos, const glm::uvec2 size) const;
        void CmdSetScissor(VkCommandBuffer commandBuffer, const glm::ivec2 offset, const VkExtent2D extent) const;
        void CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const;
        void CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const;
        void CmdSetCullMode(VkCommandBuffer commandBuffer, const VkCullModeFlags mode) const;

        NODISCARD static VkPipelineLayout CreateLayout(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutCreateInfo);
    private:
        VK_Device& m_Device;

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    };
}
