#pragma once

#include "Core.hpp"
#include "MakeInfo.hpp"
#include "VK_Config.hpp"

#define VK_CHECK_RESULT(vkFunc, errorMessage) \
if (const auto result = vkFunc; result != VK_SUCCESS) \
    Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, std::format("{} VkResult: {}.", errorMessage, static_cast<int32_t>(result)), __FILE__, __LINE__);