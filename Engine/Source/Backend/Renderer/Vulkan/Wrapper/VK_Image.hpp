#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "QueueType.hpp"

#include "vulkan.h"
#include "vk_mem_alloc.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_MemoryBuffer;

    class VK_Image
    {
    private:
        struct TransitionInfo
        {
            VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkPipelineStageFlags sourceStage = 0;
            VkPipelineStageFlags destinationStage = 0;

            VkAccessFlags srcAccessMask = 0;
            VkAccessFlags dstAccessMask = 0;

            QueueType requiredQueue = QueueType::Graphics;
        };

        static TransitionInfo DeduceTransitionInfo(const VkImageLayout oldLayout, const VkImageLayout newLayout);
        static void TransitionBarrierImpl(VkCommandBuffer commandBuffer, VkImage image,
            VkImageAspectFlags aspectFlags, const TransitionInfo& transitionInfo, const uint32_t targetMipLevel);
    public:
        static constexpr int32_t AllMips = -1;

        // this method can transition only ONE mip level at a time
        static void CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image,
            VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t targetMipLevel);

        // set targetMipLevel to AllMips (-1) to transition all mip levels
        static void TransitionLayout(VK_Image& image, const VkImageLayout newLayout, const int32_t targetMipLevel);

        VK_Image(VK_Device& device, const glm::uvec2 size, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, const uint32_t mipLevels);
        ~VK_Image();

        void CopyFromBuffer(const VK_MemoryBuffer& buffer) const;

        NODISCARD inline glm::uvec2 GetSize() const { return m_Size; }

        NODISCARD inline VkImage Get() const { return m_Image; }
        NODISCARD inline VkImageView GetView() const { return m_ImageView; }
        NODISCARD inline VkFormat GetFormat() const { return m_Format; }
        NODISCARD inline VkImageAspectFlags GetAspectFlags() const { return m_AspectFlags; }

        VK_Image(const VK_Image&) = delete;
        VK_Image operator = (const VK_Image&) = delete;
    private:
        VK_Device& m_Device;

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        std::vector<VkImageLayout> m_Layouts;

        uint32_t m_MipLevels;
        const glm::uvec2 m_Size;
        const VkFormat m_Format;
        const VkImageAspectFlags m_AspectFlags;
    };

}
