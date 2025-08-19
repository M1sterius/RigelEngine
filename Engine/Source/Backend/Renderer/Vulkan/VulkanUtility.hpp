#pragma once

#include "Core.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "Engine.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Device.hpp"
#include "Subsystems/SubsystemGetters.hpp"

#include "vulkan/vk_enum_string_helper.h"

#define VK_CHECK_RESULT(vkFunc, crashMessage) \
    if (const auto result = vkFunc; result != VK_SUCCESS) { \
        Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, \
            std::format("{}. VkResult: {}.", crashMessage, string_VkResult(result)), __FILE__, __LINE__); \
    }

namespace Rigel::Backend::Vulkan
{
    NODISCARD inline VK_Renderer& GetVKRenderer()
    {
        return GetRenderer()->GetImpl();
    }

    NODISCARD inline VK_Device& GetDevice()
    {
        return GetVKRenderer().GetDevice();
    }
}
