#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

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
        };

        static TransitionInfo DeduceTransitionInfo(const VkImageLayout oldLayout, const VkImageLayout newLayout);
    public:
        static constexpr int32_t AllMips = -1;

        /**
         * Records command to transition one image mip layer to a new layout
         */
        static void CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image,
            VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t targetMipLevel);

        VK_Image(VK_Device& device, const glm::uvec2 size, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, const uint32_t mipLevels);
        ~VK_Image();

        void CopyFromBuffer(const VK_MemoryBuffer& buffer) const;
        void TransitionLayout(const VkImageLayout newLayout, const int32_t targetMipLevel);

        NODISCARD glm::uvec2 GetSize() const { return m_Size; }

        NODISCARD VkImage Get() const { return m_Image; }
        NODISCARD VkImageView GetView() const { return m_ImageView; }
        NODISCARD VkFormat GetFormat() const { return m_Format; }
        NODISCARD VkImageAspectFlags GetAspectFlags() const { return m_AspectFlags; }
        NODISCARD uint32_t GetMipLevelCount() const { return m_MipLevels; }

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
