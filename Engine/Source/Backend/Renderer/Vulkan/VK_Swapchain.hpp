#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "VK_Device.hpp"
#include "vulkan.h"

namespace rge::backend
{
    class VK_Device;
    class VK_Semaphore;

    class VK_Swapchain
    {
    public:
        VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size);
        ~VK_Swapchain();

        VK_Swapchain(const VK_Swapchain&) = delete;
        VK_Swapchain operator = (const VK_Swapchain) = delete;

        NODISCARD uint32_t GetFramesInFlightCount() const { return m_FramesInFlight; }

        void SetupSwapchain(const glm::uvec2 requestedExtent, const bool vsyncEnabled);
        NODISCARD uint32_t AcquireNextImage(const uint32_t frameIndex, const uint64_t timeout = std::numeric_limits<uint64_t>::max());
        void Present(const uint32_t imageIndex);
    private:
        NODISCARD std::vector<VkImage> GetSwapchainImages();

        VK_Device& m_Device;
        VkSurfaceKHR m_Surface;
        glm::uvec2 m_Extent;
        uint32_t m_FramesInFlight;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

        SwapchainSupportDetails m_SwapchainSupportDetails;
        std::vector<VkImage> m_Images {};
        std::vector<VkImageView> m_ImageViews {};
        std::vector<std::unique_ptr<VK_Semaphore>> m_ImageAvailableSemaphores;

        NODISCARD static VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, const bool vsyncEnabled);
        NODISCARD static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::uvec2 extent);
        NODISCARD static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    };
}
