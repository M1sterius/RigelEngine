#include "VK_Swapchain.hpp"
#include "VK_Semaphore.hpp"
#include "VulkanUtility.hpp"
#include "Time.hpp"
#include "WindowManager.hpp"
#include "SubsystemGetters.hpp"
#include "VK_CmdBuffer.hpp"
#include "VK_Image.hpp"

NODISCARD static glm::uvec2 GetCurrentExtent()
{
    return Rigel::GetWindowManager()->GetWindowSize();
}

NODISCARD static bool GetCurrentVsyncSetting()
{
    return Rigel::GetWindowManager()->IsVsyncEnabled();
}

namespace Rigel::Backend::Vulkan
{
    VK_Swapchain::VK_Swapchain(VK_Device& device, VkSurfaceKHR surface, const glm::uvec2 size)
        : m_Device(device), m_Surface(surface), m_Extent(size)
    {
        Debug::Trace("Creating Vulkan swapchain.");
        m_SwapchainSupportDetails = m_Device.GetSwapchainSupportDetails();
        m_FramesInFlight = m_SwapchainSupportDetails.Capabilities.minImageCount + 1;

        for (uint32_t i = 0; i < m_FramesInFlight; i++)
        {
            m_ImageAvailableSemaphores.emplace_back(std::make_unique<VK_Semaphore>(m_Device));
            m_TransitionCompleteSemaphores.emplace_back(std::make_unique<VK_Semaphore>(m_Device));
            m_AcquireTransitionCommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device));
            m_PresentTransitionCommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device));
        }

        SetupSwapchain(m_Extent, GetCurrentVsyncSetting());
    }

    VK_Swapchain::~VK_Swapchain()
    {
        Debug::Trace("Destroying vulkan swapchain.");

        for (const auto& view : m_ImageViews)
            vkDestroyImageView(m_Device.Get(), view, nullptr);
        vkDestroySwapchainKHR(m_Device.Get(), m_Swapchain, nullptr);
    }

    AcquireImageInfo VK_Swapchain::AcquireNextImage()
    {
        const auto frameIndex = Time::GetFrameCount() % GetFramesInFlightCount();

        uint32_t imageIndex;
        if (const auto result = vkAcquireNextImageKHR(m_Device.Get(), m_Swapchain, std::numeric_limits<uint64_t>::max(),
                                      m_ImageAvailableSemaphores[frameIndex]->Get(), VK_NULL_HANDLE, &imageIndex);
        result != VK_SUCCESS)
        {
            if (result == VK_ERROR_OUT_OF_DATE_KHR)
                SetupSwapchain(GetCurrentExtent(), GetCurrentVsyncSetting());
            else if (result != VK_SUBOPTIMAL_KHR)
            {
                Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to acquire next vulkan swapchain image!. VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
            }
        }

        const auto& cmdBuff = m_AcquireTransitionCommandBuffers[frameIndex];

        cmdBuff->Reset(0);
        cmdBuff->BeginRecording(0);
        VK_Image::CmdTransitionLayout(cmdBuff->Get(), m_Images[imageIndex], m_SwapchainImageFormat,
            VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        cmdBuff->EndRecording();

        constexpr VkPipelineStageFlags stageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        const VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[frameIndex]->Get() };
        const VkSemaphore signalSemaphores[] = { m_TransitionCompleteSemaphores[frameIndex]->Get() };
        const auto buff = cmdBuff->Get();

        auto submitInfo = MakeInfo<VkSubmitInfo>();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buff;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        submitInfo.pWaitDstStageMask = stageFlags;

        m_Device.SubmitGraphicsQueue(1, &submitInfo, VK_NULL_HANDLE);

        return {imageIndex, m_Images[imageIndex], m_ImageViews[imageIndex], m_TransitionCompleteSemaphores[frameIndex]->Get()};
    }

    void VK_Swapchain::Present(const uint32_t imageIndex, VkSemaphore renderDoneSemaphore)
    {
        const auto frameIndex = Time::GetFrameCount() % GetFramesInFlightCount();
        const auto& cmdBuff = m_PresentTransitionCommandBuffers[frameIndex];

        cmdBuff->Reset(0);
        cmdBuff->BeginRecording(0);
        VK_Image::CmdTransitionLayout(cmdBuff->Get(), m_Images[imageIndex], m_SwapchainImageFormat,
            VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        cmdBuff->EndRecording();

        constexpr VkPipelineStageFlags stageFlags[] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
        const VkSemaphore transitionDoneSemaphores[] = { m_TransitionCompleteSemaphores[frameIndex]->Get() };
        const auto buff = cmdBuff->Get();

        auto submitInfo = MakeInfo<VkSubmitInfo>();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buff;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &renderDoneSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = transitionDoneSemaphores;
        submitInfo.pWaitDstStageMask = stageFlags;

        m_Device.SubmitGraphicsQueue(1, &submitInfo, VK_NULL_HANDLE);

        auto presentInfo = MakeInfo<VkPresentInfoKHR>();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = transitionDoneSemaphores;

        if (const auto result = m_Device.SubmitPresentQueue(&presentInfo); result != VK_SUCCESS)
        {
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
                SetupSwapchain(GetCurrentExtent(), GetCurrentVsyncSetting());
            else
            {
                Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to present Vulkan image! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
            }
        }
    }

    void VK_Swapchain::SetupSwapchain(const glm::uvec2 requestedExtent, const bool vsyncEnabled)
    {
        m_Device.WaitIdle();

        // NEVER!! cache the swapchain support details because they can change after a window resize
        const auto& supportDetails = m_Device.GetSwapchainSupportDetails();

        const auto format = ChooseSwapchainSurfaceFormat(supportDetails.Formats);
        const auto presentMode = ChooseSwapchainPresentMode(supportDetails.PresentModes, vsyncEnabled);
        const auto extent = ChooseSwapchainExtent(supportDetails.Capabilities, requestedExtent);

        m_SwapchainImageFormat = format.format;

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

        const auto [graphicsFamily, presentFamily, _] = m_Device.GetQueueFamilyIndices();
        const uint32_t queueFamilyIndices[] = {graphicsFamily.value(), presentFamily.value()};

        if (graphicsFamily != presentFamily)
        {
            // If graphics and present queues are not in the same family, we want
            // resources to be freely shared between them, so VK_SHARING_MODE_CONCURRENT is chosen in that case

            // Note that we don't have to share images with the dedicated transfer queue
            // because there is no reason fo it to touch the image

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = supportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // specifies how the window will blend with other windows
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        m_Extent = {extent.width, extent.height};
        const auto oldSwapchain = m_Swapchain;
        createInfo.oldSwapchain = oldSwapchain;

        VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device.Get(), &createInfo, nullptr, &m_Swapchain),
            "Failed to create Vulkan swapchain!");

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

            VK_CHECK_RESULT(vkCreateImageView(m_Device.Get(), &imageViewCreateInfo, nullptr, &m_ImageViews[i]), "Failed to create swapchain image view!");
        }
    }

    std::vector<VkImage> VK_Swapchain::GetSwapchainImages() const
    {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Device.Get(), m_Swapchain, &imageCount, nullptr);

        if (imageCount == 0)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, "Vulkan swapchain image count is 0!", __FILE__, __LINE__);
        }

        auto images = std::vector<VkImage>(imageCount);
        vkGetSwapchainImagesKHR(m_Device.Get(), m_Swapchain, &imageCount, images.data());

        return images;
    }

#pragma region SwapchainUtilityFunctions
    VkPresentModeKHR VK_Swapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes,
                                             const bool vsyncEnabled)
    {
        if (vsyncEnabled)
            return VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& mode : availablePresentModes)
        {
            if (mode == VK_Config::PrioritySwapchainPresentMode)
                return mode;
        }

        Debug::Warning("Priority swapchain present mode {} is not supported.",
            string_VkPresentModeKHR(VK_Config::PrioritySwapchainPresentMode));

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
