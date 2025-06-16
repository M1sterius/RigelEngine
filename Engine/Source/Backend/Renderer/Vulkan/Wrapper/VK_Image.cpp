#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_CmdBuffer.hpp"
#include "VulkanException.hpp"
#include "VK_Device.hpp"
#include "VK_Fence.hpp"
#include "MakeInfo.hpp"

namespace Rigel::Backend::Vulkan
{
    void VK_Image::CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
        VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        auto barrier = MakeInfo<VkImageMemoryBarrier>();
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = 0;

            sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = 0;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw VulkanException("Unsupported Vulkan image layout transition!", VK_ERROR_UNKNOWN);
        }

        vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
        );
    }

    void VK_Image::TransitionLayout(VK_Device& device, const VK_Image& image, VkFormat format, VkImageLayout oldLayout,
            VkImageLayout newLayout)
    {
        const auto commandBuffer = VK_CmdBuffer(device);
        commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        CmdTransitionLayout(commandBuffer.Get(), image.Get(), format, image.GetAspectFlags(), oldLayout, newLayout);

        commandBuffer.EndRecording();
        auto submitInfo = MakeInfo<VkSubmitInfo>();
        submitInfo.commandBufferCount = 1;
        const auto buff = commandBuffer.Get();
        submitInfo.pCommandBuffers = &buff;

        // This avoids stalling the entire queue (so don't use vkQueueWaitIdle)
        const auto fence = std::make_unique<VK_Fence>(device);
        device.SubmitGraphicsQueue(1, &submitInfo, fence->Get());
        fence->Wait();
    }

    VK_Image::VK_Image(VK_Device& device, const glm::uvec2 size, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkImageAspectFlags aspectFlags)
        : m_Device(device), m_Size(size), m_Format(format), m_AspectFlags(aspectFlags)
    {
        ASSERT(m_Size.x > 0 && m_Size.y > 0, "Cannot create an image of zero size");

        auto imageInfo = MakeInfo<VkImageCreateInfo>();
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1; // TODO: Implement support for mipmap levels
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_Format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (const auto result = vmaCreateImage(m_Device.GetVmaAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create vulkan image. VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }

        auto viewInfo = MakeInfo<VkImageViewCreateInfo>();
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_Format;
        viewInfo.subresourceRange.aspectMask = m_AspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (const auto result = vkCreateImageView(m_Device.Get(), &viewInfo, nullptr, &m_ImageView); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create vulkan image view. VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }
    }

    VK_Image::~VK_Image()
    {
        vkDestroyImageView(m_Device.Get(), m_ImageView, nullptr);
        vmaDestroyImage(m_Device.GetVmaAllocator(), m_Image, m_Allocation);
    }

    void VK_Image::CopyFromBuffer(const VK_MemoryBuffer& buffer)
    {
        const auto cmdBuffer = VK_CmdBuffer::BeginSingleTime(m_Device);

        VkBufferImageCopy region {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            m_Size.x,
            m_Size.y,
            1
        };

        vkCmdCopyBufferToImage(cmdBuffer->Get(), buffer.Get(), m_Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VK_CmdBuffer::EndSingleTime(m_Device, *cmdBuffer);
    }
}
