#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan.h"

namespace Rigel::Backend
{
    class VK_Device;

    class VK_Image 
    {
    public:
        static void CmdTransitionLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
        static void TransitionLayout(VK_Device& device, const VK_Image& image, VkFormat format,
            VkImageLayout oldLayout, VkImageLayout newLayout);

        VK_Image(VK_Device& device, const glm::uvec2 size, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage, VkImageAspectFlags aspectFlags);
        ~VK_Image();

        NODISCARD inline VkImage Get() const { return m_Image; }
        NODISCARD inline VkImageView GetView() const { return m_ImageView; }
        NODISCARD inline glm::uvec2 GetSize() const { return m_Size; }
        NODISCARD inline VkFormat GetFormat() const { return m_Format; }
        NODISCARD inline VkImageAspectFlags GetAspectFlags() const { return m_AspectFlags; }

        VK_Image(const VK_Image&) = delete;
        VK_Image operator = (const VK_Image&) = delete;
    private:
        VK_Device& m_Device;

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;

        const glm::uvec2 m_Size;
        const VkFormat m_Format;
        const VkImageAspectFlags m_AspectFlags;
    };

}
