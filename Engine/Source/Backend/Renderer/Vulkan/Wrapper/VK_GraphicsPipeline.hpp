#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <memory>

namespace rge::backend
{
    class VK_Device;
    class VK_Shader;

    class VK_GraphicsPipeline 
    {
    public:
        VK_GraphicsPipeline(VK_Device& device, const VkPipelineLayoutCreateInfo& layoutInfo, VkGraphicsPipelineCreateInfo& pipelineInfo);
        ~VK_GraphicsPipeline();

        NODISCARD inline VkPipeline Get() const { return m_GraphicsPipeline; }
        NODISCARD inline VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

        NODISCARD static std::unique_ptr<VK_GraphicsPipeline> CreateDefaultGraphicsPipeline(VK_Device& device, const VkFormat swapchainImageFormat, const VK_Shader& shader);
    private:
        VK_Device& m_Device;

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    };
}
