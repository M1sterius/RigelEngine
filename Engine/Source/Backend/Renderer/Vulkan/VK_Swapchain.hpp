#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "VK_Device.hpp"
#include "vulkan.h"

namespace rge::backend
{

    class VK_Swapchain
    {
    public:
        VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size);
        ~VK_Swapchain();

        VK_Swapchain(const VK_Swapchain&) = delete;
        VK_Swapchain operator = (const VK_Swapchain) = delete;
    private:
        VK_Device& m_Device;
        VkSurfaceKHR m_Surface;
        VkExtent2D m_Extent;

        SwapchainSupportDetails m_SwapchainSupportDetails;

        NODISCARD static VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, const bool vsyncEnabled);
        NODISCARD static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D extent);
        NODISCARD static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    };
}
