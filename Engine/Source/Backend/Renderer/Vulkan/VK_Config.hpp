#pragma once

#include "Core.hpp"
#include "Exceptions.hpp"
#include "vulkan.h"

#include <format>
#include <vector>
#include <array>

namespace Rigel::Backend::Vulkan
{
    class VK_Config
    {
    public:
    #ifdef DEBUG_BUILD
        static constexpr bool EnableValidationLayers = true;
    #else
        static constexpr bool EnableValidationLayers = false;
    #endif

        static constexpr auto MinimalRequiredAPIVersion = VK_API_VERSION_1_4;

        inline static const std::vector<const char*> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // Put instance extensions you need to be enabled here
        inline static const std::vector<const char*> RequiredInstanceExtensions = {

        };

        // Put physical device extensions you need to be enabled here
        inline static const std::vector<const char*> RequiredPhysicalDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
        };

        inline static VkPresentModeKHR PrioritySwapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        inline static VkFormat PrioritySwapchainSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB; // VK_FORMAT_B8G8R8A8_UNORM
        inline static VkColorSpaceKHR PrioritySwapchainSurfaceColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    };
}
