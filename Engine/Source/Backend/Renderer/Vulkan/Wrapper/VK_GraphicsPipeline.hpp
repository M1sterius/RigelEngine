#pragma once

#include "Core.hpp"
#include "Assets/Shader.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Shader;

    struct GraphicsPipelineConfigInfo
    {

    };

    class VK_GraphicsPipeline 
    {
    public:
        VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo);
        ~VK_GraphicsPipeline();

        VK_GraphicsPipeline(const VK_GraphicsPipeline&) = delete;
        VK_GraphicsPipeline operator = (const VK_GraphicsPipeline&) = delete;

        NODISCARD inline VkPipeline Get() const { return m_GraphicsPipeline; }
        NODISCARD inline VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

        void CmdBind(VkCommandBuffer commandBuffer) const;
        void CmdSetViewport(VkCommandBuffer commandBuffer, const glm::vec2 pos, const glm::uvec2 size) const;
        void CmdSetScissor(VkCommandBuffer commandBuffer, const glm::ivec2 offset, const VkExtent2D extent) const;
        void CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const;
        void CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, const VkBool32 enabled) const;
        void CmdSetCullMode(VkCommandBuffer commandBuffer, const VkCullModeFlags mode) const;

        NODISCARD static std::unique_ptr<VK_GraphicsPipeline> CreateDefaultGraphicsPipeline(VK_Device& device, const VkFormat colorAttachmentFormat,
            const Shader::Variant& shaderVariant, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);

        NODISCARD static std::unique_ptr<VK_GraphicsPipeline> CreateGeometryPassPipeline(VK_Device& device,
            const VkPipelineLayoutCreateInfo& pipelineLayoutInfo, const Shader::Variant& shader);
    private:
        VK_Device& m_Device;

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    };
}
