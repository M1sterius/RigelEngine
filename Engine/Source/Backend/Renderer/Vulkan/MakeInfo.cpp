#include "MakeInfo.hpp"
#include "vulkan.h"

namespace rge::backend
{
    template <>
    VkApplicationInfo MakeInfo(VkApplicationInfo info) {
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        return info;
    }

    template <>
    VkBufferCreateInfo MakeInfo(VkBufferCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        return info;
    }

    template <>
    VkCommandBufferAllocateInfo MakeInfo(VkCommandBufferAllocateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        return info;
    }

    template <>
    VkCommandBufferBeginInfo MakeInfo(VkCommandBufferBeginInfo info) {
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        return info;
    }

    template <>
    VkCommandPoolCreateInfo MakeInfo(VkCommandPoolCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        return info;
    }

    template <>
    VkDebugMarkerMarkerInfoEXT MakeInfo(VkDebugMarkerMarkerInfoEXT info) {
        info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        return info;
    }

    template <>
    VkDebugMarkerObjectNameInfoEXT MakeInfo(VkDebugMarkerObjectNameInfoEXT info) {
        info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
        return info;
    }

    template <>
    VkDebugMarkerObjectTagInfoEXT MakeInfo(VkDebugMarkerObjectTagInfoEXT info) {
        info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
        return info;
    }

    template <>
    VkDebugReportCallbackCreateInfoEXT MakeInfo(VkDebugReportCallbackCreateInfoEXT info) {
        info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        return info;
    }

    template <>
    VkDescriptorPoolCreateInfo MakeInfo(VkDescriptorPoolCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        return info;
    }

    template <>
    VkDescriptorSetAllocateInfo MakeInfo(VkDescriptorSetAllocateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        return info;
    }

    template <>
    VkDescriptorSetLayoutCreateInfo MakeInfo(VkDescriptorSetLayoutCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        return info;
    }

    template <>
    VkDeviceCreateInfo MakeInfo(VkDeviceCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        return info;
    }

    template <>
    VkDeviceQueueCreateInfo MakeInfo(VkDeviceQueueCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        return info;
    }

    template <>
    VkFenceCreateInfo MakeInfo(VkFenceCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        return info;
    }

    template <>
    VkFramebufferCreateInfo MakeInfo(VkFramebufferCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        return info;
    }

    template <>
    VkGraphicsPipelineCreateInfo MakeInfo(VkGraphicsPipelineCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        return info;
    }

    template <>
    VkImageCreateInfo MakeInfo(VkImageCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        return info;
    }

    template <>
    VkImageMemoryBarrier MakeInfo(VkImageMemoryBarrier info) {
        info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        return info;
    }

    template <>
    VkImageViewCreateInfo MakeInfo(VkImageViewCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        return info;
    }

    template <>
    VkInstanceCreateInfo MakeInfo(VkInstanceCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        return info;
    }

    template <>
    VkMemoryBarrier MakeInfo(VkMemoryBarrier info) {
        info.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        return info;
    }

    template <>
    VkPipelineColorBlendStateCreateInfo MakeInfo(VkPipelineColorBlendStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineDepthStencilStateCreateInfo MakeInfo(VkPipelineDepthStencilStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineInputAssemblyStateCreateInfo MakeInfo(VkPipelineInputAssemblyStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineLayoutCreateInfo MakeInfo(VkPipelineLayoutCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineMultisampleStateCreateInfo MakeInfo(VkPipelineMultisampleStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineRasterizationStateCreateInfo MakeInfo(VkPipelineRasterizationStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineShaderStageCreateInfo MakeInfo(VkPipelineShaderStageCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineVertexInputStateCreateInfo MakeInfo(VkPipelineVertexInputStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineViewportStateCreateInfo MakeInfo(VkPipelineViewportStateCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkPresentInfoKHR MakeInfo(VkPresentInfoKHR info) {
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        return info;
    }

    template <>
    VkRenderPassBeginInfo MakeInfo(VkRenderPassBeginInfo info) {
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        return info;
    }

    template <>
    VkRenderPassCreateInfo MakeInfo(VkRenderPassCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        return info;
    }

    template <>
    VkSamplerCreateInfo MakeInfo(VkSamplerCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        return info;
    }

    template <>
    VkSemaphoreCreateInfo MakeInfo(VkSemaphoreCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        return info;
    }

    template <>
    VkShaderModuleCreateInfo MakeInfo(VkShaderModuleCreateInfo info) {
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        return info;
    }

    template <>
    VkSubmitInfo MakeInfo(VkSubmitInfo info) {
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        return info;
    }

    template <>
    VkSwapchainCreateInfoKHR MakeInfo(VkSwapchainCreateInfoKHR info) {
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        return info;
    }

    template <>
    VkWriteDescriptorSet MakeInfo(VkWriteDescriptorSet info) {
        info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        return info;
    }

    template <>
    VkPipelineRenderingCreateInfo MakeInfo(VkPipelineRenderingCreateInfo info)
    {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        return info;
    }

    template <>
    VkPipelineDynamicStateCreateInfo MakeInfo(VkPipelineDynamicStateCreateInfo info)
    {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        return info;
    }

    template <>
    VkMemoryAllocateInfo MakeInfo(VkMemoryAllocateInfo info)
    {
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        return info;
    }
}
