#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <array>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Shader;

    class VK_GraphicsPipeline 
    {
    public:
        VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo);
        ~VK_GraphicsPipeline();

        VK_GraphicsPipeline(const VK_GraphicsPipeline&) = delete;
        VK_GraphicsPipeline operator = (const VK_GraphicsPipeline&) = delete;

        NODISCARD inline VkPipeline Get() const { return m_GraphicsPipeline; }
        NODISCARD inline VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

        NODISCARD static std::unique_ptr<VK_GraphicsPipeline> CreateDefaultGraphicsPipeline(VK_Device& device, const VkFormat swapchainImageFormat,
            const std::array<VkPipelineShaderStageCreateInfo, 2>& shaderStages, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    private:
        VK_Device& m_Device;

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    };
}
