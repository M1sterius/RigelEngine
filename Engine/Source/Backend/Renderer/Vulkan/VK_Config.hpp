#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <vector>
#include <array>

namespace rge::backend
{
#ifdef DEBUG_BUILD
    constexpr bool EnableValidationLayers = true;
#else
    constexpr bool EnableValidationLayers = false;
#endif

    static const auto MinimalRequiredVulkanVersion = VK_API_VERSION_1_3;

    static const std::vector<const char*> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // Put instance extensions you need to be enabled here
    static const std::vector<VkExtensionProperties> RequiredInstanceExtensions = {

    };

    // Put physical device extensions you need to be enabled here
    static const std::vector<VkPhysicalDeviceFeatures> RequiredPhysicalDeviceFeatures = {

    };
}
