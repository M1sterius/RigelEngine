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

        void SetupSwapchain(const glm::uvec2 requestedExtent, const bool vsyncEnabled);
    private:
        NODISCARD std::vector<VkImage> GetSwapchainImages();

        VK_Device& m_Device;
        VkSurfaceKHR m_Surface;
        glm::uvec2 m_Extent;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

        SwapchainSupportDetails m_SwapchainSupportDetails;
        std::vector<VkImage> m_Images {};
        std::vector<VkImageView> m_ImageViews {};

        NODISCARD static VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, const bool vsyncEnabled);
        NODISCARD static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::uvec2 extent);
        NODISCARD static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    };
}
