#include "VK_Swapchain.hpp"
#include "VK_Config.hpp"

#include <numeric>

namespace rge::backend
{
    VK_Swapchain::VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size)
        : m_Device(device), m_Surface(surface), m_Extent({size.x, size.y})
    {
        m_SwapchainSupportDetails = m_Device.GetSwapchainSupportDetails();
    }

    VK_Swapchain::~VK_Swapchain()
    {

    }

#pragma region SwapchainUtilityFunctions
    VkPresentModeKHR VK_Swapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes,
                                             const bool vsyncEnabled)
    {
        if (vsyncEnabled) return VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& mode : availablePresentModes)
            if (mode == VK_Config::PriorityPresentMode) return mode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VK_Swapchain::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D extent)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent; // What does it do?

        VkExtent2D actualExtent = {extent.width, extent.height};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    VkSurfaceFormatKHR VK_Swapchain::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        ASSERT(!availableFormats.empty(), "No Vulkan swapchain surface formats available!");

        // If the device supports only one format with `VK_FORMAT_UNDEFINED`, we can the preferred format.
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
