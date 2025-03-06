#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <vector>
#include <array>

namespace rge::backend
{
    class VK_Config
    {
    public:
        #ifdef DEBUG_BUILD
        static constexpr bool EnableValidationLayers = true;
        #else
        static constexpr bool EnableValidationLayers = false;
        #endif

        static constexpr auto MinimalRequiredVulkanVersion = VK_API_VERSION_1_3;

        inline static const std::vector<const char*> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // Put instance extensions you need to be enabled here
        inline static const std::vector<const char*> RequiredInstanceExtensions = {

        };

        // Put physical device extensions you need to be enabled here
        inline static const std::vector<const char*> RequiredPhysicalDeviceFeatures = {

        };
    };
}
