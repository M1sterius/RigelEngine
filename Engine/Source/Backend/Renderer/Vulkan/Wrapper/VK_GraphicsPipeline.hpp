#pragma once

#include "Core.hpp"
#include "Assets/Shader.hpp"
#include "Handles/AssetHandle.hpp"
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
        NODISCARD static GraphicsPipelineConfigInfo Make();

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
    private:
        GraphicsPipelineConfigInfo() = default;
    };

    class VK_GraphicsPipeline 
    {
    public:
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
