#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <vector>
#include <array>

namespace rge_bcknd
{
#ifdef DEBUG_BUILD
    constexpr bool EnableValidationLayers = true;
#else
    constexpr bool EnableValidationLayers = false;
#endif

    static const std::vector<const char*> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
}
