#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan.h"
#include "vk_mem_alloc.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_MemoryBuffer;

    class VK_Image 
    {
    public:
        // set targetMipLevel to -1 to transition all levels
        static void CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, int32_t targetMipLevel);
        static void TransitionLayout(VK_Image& image, VkImageLayout newLayout);

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
        VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;

        uint32_t m_MipLevels;
        const glm::uvec2 m_Size;
        const VkFormat m_Format;
        const VkImageAspectFlags m_AspectFlags;
    };

}
