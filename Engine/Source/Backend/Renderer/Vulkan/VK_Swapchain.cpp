#include "VK_Swapchain.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

#include <numeric>

namespace rge::backend
{
    VK_Swapchain::VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size)
        : m_Device(device), m_Surface(surface), m_Extent(size)
    {
        RGE_TRACE("Creating Vulkan swapchain.");
        m_SwapchainSupportDetails = m_Device.GetSwapchainSupportDetails();

        SetupSwapchain(m_Extent, false); // TODO: Change disabled vsync to use window manager setting
    }

    VK_Swapchain::~VK_Swapchain()
    {
        for (const auto& view : m_ImageViews)
            vkDestroyImageView(m_Device.Get(), view, nullptr);
        vkDestroySwapchainKHR(m_Device.Get(), m_Swapchain, nullptr);
    }

    void VK_Swapchain::SetupSwapchain(const glm::uvec2 requestedExtent, const bool vsyncEnabled)
    {
        const auto& supportDetails = m_SwapchainSupportDetails;

        const auto format = ChooseSwapchainSurfaceFormat(supportDetails.Formats);
        const auto presentMode = ChooseSwapchainPresentMode(supportDetails.PresentModes, vsyncEnabled);
        const auto extent = ChooseSwapchainExtent(supportDetails.Capabilities, requestedExtent);

        auto imageCount = supportDetails.Capabilities.minImageCount + 1;
        if (supportDetails.Capabilities.maxImageCount > 0 && imageCount > supportDetails.Capabilities.maxImageCount)
            imageCount = supportDetails.Capabilities.maxImageCount;

        auto createInfo = MakeInfo<VkSwapchainCreateInfoKHR>();
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = format.format;
        createInfo.imageColorSpace = format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const auto indices = m_Device.GetQueueFamilyIndices();
        uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            // If graphics and present queues are not in the same family, we want
            // resources to be freely shared between them, so VK_SHARING_MODE_CONCURRENT is chosen oin that case

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = supportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // specifies how the window will blend with other windows
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        m_Extent = {extent.width, extent.height};
        const auto oldSwapchain = m_Swapchain;
        createInfo.oldSwapchain = oldSwapchain; // Maybe that will cause problems? I'm not sure right now

        if (const auto result = vkCreateSwapchainKHR(m_Device.Get(), &createInfo, nullptr, &m_Swapchain); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan swapchain!", result);

        // Clean up old swapchain and associated resources if needed
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            for (const auto& view : m_ImageViews)
                vkDestroyImageView(m_Device.Get(), view, nullptr);

            m_Images.clear();
            m_ImageViews.clear();

            vkDestroySwapchainKHR(m_Device.Get(), oldSwapchain, nullptr);
        }

        m_Images = GetSwapchainImages();
        m_ImageViews.resize(m_Images.size());

        // Create new image views for each swapchain image
        for (size_t i = 0; i < m_ImageViews.size(); i++)
        {
            auto imageViewCreateInfo = MakeInfo<VkImageViewCreateInfo>();
            imageViewCreateInfo.image = m_Images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = format.format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            if (const auto result = vkCreateImageView(m_Device.Get(), &imageViewCreateInfo, nullptr, &m_ImageViews[i]); result != VK_SUCCESS)
                throw VulkanException("Failed to create swapchain image view!", result);
        }
    }

    std::vector<VkImage> VK_Swapchain::GetSwapchainImages()
    {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Device.Get(), m_Swapchain, &imageCount, nullptr);

        if (imageCount == 0)
            throw VulkanException("Swapchain image count is 0.", UNKNOWN_VK_RESULT);

        auto images = std::vector<VkImage>(imageCount);
        vkGetSwapchainImagesKHR(m_Device.Get(), m_Swapchain, &imageCount, images.data());

        return images;
    }

#pragma region SwapchainUtilityFunctions
    VkPresentModeKHR VK_Swapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes,
                                             const bool vsyncEnabled)
    {
        if (vsyncEnabled) return VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& mode : availablePresentModes)
            if (mode == VK_Config::PrioritySwapchainPresentMode) return mode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VK_Swapchain::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::uvec2 extent)
    {
        // Checks if the extent is already predetermined by Vulkan
        // Which can happen in fullscreen mode for example
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        VkExtent2D actualExtent = {extent.x, extent.y};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    VkSurfaceFormatKHR VK_Swapchain::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        ASSERT(!availableFormats.empty(), "No Vulkan swapchain surface formats available!");

        // If the device supports only one format with `VK_FORMAT_UNDEFINED`, we can use the preferred format.
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
            return {VK_Config::PrioritySwapchainSurfaceFormat, VK_Config::PrioritySwapchainSurfaceColorSpace};

        for (const auto& format : availableFormats)
        {
            if (format.format == VK_Config::PrioritySwapchainSurfaceFormat &&
                format.colorSpace == VK_Config::PrioritySwapchainSurfaceColorSpace)
            {
                return {VK_Config::PrioritySwapchainSurfaceFormat, VK_Config::PrioritySwapchainSurfaceColorSpace};
            }
        }

        return availableFormats[0];
    }
#pragma endregion
}
