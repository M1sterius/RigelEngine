#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "VK_Device.hpp"
#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Semaphore;
    class VK_CmdBuffer;

    struct AcquireImageInfo
    {
        uint32_t imageIndex;
        VkImage image;
        VkImageView imageView;
        VkSemaphore availableSemaphore;
    };

    class VK_Swapchain
    {
    public:
        VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size);
        ~VK_Swapchain();

        VK_Swapchain(const VK_Swapchain&) = delete;
        VK_Swapchain operator = (const VK_Swapchain) = delete;

        NODISCARD uint32_t GetFramesInFlightCount() const { return m_FramesInFlight; }
        NODISCARD uint32_t GetMinImageCount() const { return m_SwapchainSupportDetails.Capabilities.minImageCount; }
        NODISCARD std::vector<VkImage>& GetImages() { return m_Images; }
        NODISCARD std::vector<VkImageView>& GetImageViews() { return m_ImageViews; }
        NODISCARD VkFormat GetSwapchainImageFormat() const { return m_SwapchainImageFormat; }
        NODISCARD VkExtent2D GetExtent() const { return {m_Extent.x, m_Extent.y}; }

        void SetupSwapchain(const glm::uvec2 requestedExtent, const bool vsyncEnabled);

        NODISCARD AcquireImageInfo AcquireNextImage();
        void Present(const uint32_t imageIndex, VkSemaphore renderDoneSemaphore);
    private:
        NODISCARD std::vector<VkImage> GetSwapchainImages() const;

        VK_Device& m_Device;
        VkSurfaceKHR m_Surface;
        glm::uvec2 m_Extent;
        uint32_t m_FramesInFlight;

        VkFormat m_SwapchainImageFormat {};

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

        SwapchainSupportDetails m_SwapchainSupportDetails;
        std::vector<VkImage> m_Images {};
        std::vector<VkImageView> m_ImageViews {};

        std::vector<std::unique_ptr<VK_CmdBuffer>> m_AcquireTransitionCommandBuffers;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_PresentTransitionCommandBuffers;
        std::vector<std::unique_ptr<VK_Semaphore>> m_ImageAvailableSemaphores;
        std::vector<std::unique_ptr<VK_Semaphore>> m_TransitionCompleteSemaphores;

        NODISCARD static VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, const bool vsyncEnabled);
        NODISCARD static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::uvec2 extent);
        NODISCARD static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    };
}
