#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_CmdBuffer.hpp"
#include "VulkanUtility.hpp"
#include "VK_Fence.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_Image::TransitionInfo VK_Image::DeduceTransitionInfo(const VkImageLayout oldLayout,
        const VkImageLayout newLayout)
    {
        auto transitionInfo = TransitionInfo();
        transitionInfo.oldLayout = oldLayout;
        transitionInfo.newLayout = newLayout;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            transitionInfo.srcAccessMask = 0;
            transitionInfo.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

            transitionInfo.requiredQueue = QueueType::Transfer;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            transitionInfo.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            transitionInfo.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

            transitionInfo.requiredQueue = QueueType::Transfer;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            transitionInfo.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            transitionInfo.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            transitionInfo.srcAccessMask = 0;
            transitionInfo.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            transitionInfo.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            transitionInfo.dstAccessMask = 0;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            transitionInfo.srcAccessMask = 0;
            transitionInfo.dstAccessMask = 0;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        {
            transitionInfo.srcAccessMask = 0;
            transitionInfo.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            transitionInfo.sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            transitionInfo.destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, "Unsupported vulkan image layout transition!", __FILE__, __LINE__);
        }

        return transitionInfo;
    }

    void VK_Image::TransitionBarrierImpl(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectFlags,
        const TransitionInfo& transitionInfo, const uint32_t targetMipLevel)
    {
        auto barrier = MakeInfo<VkImageMemoryBarrier>();
        barrier.oldLayout = transitionInfo.oldLayout;
        barrier.newLayout = transitionInfo.newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseMipLevel = targetMipLevel;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = transitionInfo.srcAccessMask;
        barrier.dstAccessMask = transitionInfo.dstAccessMask;

        vkCmdPipelineBarrier(
        commandBuffer,
        transitionInfo.sourceStage, transitionInfo.destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
        );
    }

    void VK_Image::CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image,
        VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t targetMipLevel)
    {
        if (oldLayout == newLayout)
            return;

        const auto transitionInfo = DeduceTransitionInfo(oldLayout, newLayout);
        TransitionBarrierImpl(commandBuffer, image, aspectFlags, transitionInfo, targetMipLevel);
    }

    void VK_Image::TransitionLayout(VK_Image& image, const VkImageLayout newLayout, const int32_t targetMipLevel)
    {
        ASSERT(targetMipLevel >= -1 && targetMipLevel <= static_cast<int32_t>(image.m_MipLevels), "Invalid mip level!");

        // transition all mip levels
        if (targetMipLevel == -1)
        {
            const auto cmdBuff = VK_CmdBuffer::BeginSingleTime(image.m_Device, QueueType::Graphics);
            for (uint32_t i = 0; i < image.m_MipLevels; ++i)
            {
                auto& oldLayout = image.m_Layouts[i];

                if (oldLayout == newLayout)
                    continue;

                CmdTransitionLayout(cmdBuff->Get(), image.m_Image,
                    image.m_AspectFlags, oldLayout, newLayout, i);
                oldLayout = newLayout;
            }
            VK_CmdBuffer::EndSingleTime(image.m_Device, *cmdBuff);
        }
        // transition only one mip level
        else
        {
            auto& oldLayout = image.m_Layouts[targetMipLevel];

            if (oldLayout == newLayout)
                return;

            const auto transitionInfo = DeduceTransitionInfo(oldLayout, newLayout);
            const auto cmdBuff = VK_CmdBuffer::BeginSingleTime(image.m_Device, transitionInfo.requiredQueue);

            TransitionBarrierImpl(cmdBuff->Get(), image.m_Image,
                image.m_AspectFlags, transitionInfo, targetMipLevel);
            oldLayout = newLayout;

            VK_CmdBuffer::EndSingleTime(image.m_Device, *cmdBuff);
        }
    }

    VK_Image::VK_Image(VK_Device& device, const glm::uvec2 size, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, const uint32_t mipLevels)
        : m_Device(device), m_MipLevels(mipLevels), m_Size(size), m_Format(format), m_AspectFlags(aspectFlags)
    {
        ASSERT(m_Size.x > 0 && m_Size.y > 0, "Image cannot have zero size");

        auto imageInfo = MakeInfo<VkImageCreateInfo>();
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = m_MipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_Format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_Layouts = std::vector(m_MipLevels, VK_IMAGE_LAYOUT_UNDEFINED);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VK_CHECK_RESULT(vmaCreateImage(m_Device.GetVmaAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr), "Failed to create vma image!");

        auto viewInfo = MakeInfo<VkImageViewCreateInfo>();
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_Format;
        viewInfo.subresourceRange.aspectMask = m_AspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = m_MipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK_RESULT(vkCreateImageView(m_Device.Get(), &viewInfo, nullptr, &m_ImageView), "Failed to create image view!");
    }

    VK_Image::~VK_Image()
    {
        vkDestroyImageView(m_Device.Get(), m_ImageView, nullptr);
        vmaDestroyImage(m_Device.GetVmaAllocator(), m_Image, m_Allocation);
    }

    void VK_Image::CopyFromBuffer(const VK_MemoryBuffer& buffer) const
    {
        const auto cmdBuffer = VK_CmdBuffer::BeginSingleTime(m_Device, QueueType::Transfer);

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
