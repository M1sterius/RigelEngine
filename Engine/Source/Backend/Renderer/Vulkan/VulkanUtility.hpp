#pragma once

#include "Core.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Device.hpp"

#include "vulkan/vulkan.h"

#define VK_CHECK_RESULT(vkFunc, crashMessage) \
    if (const auto result = vkFunc; result != VK_SUCCESS) { \
        Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, \
            std::format("{}. VkResult: {}.", crashMessage, static_cast<int32_t>(result)), __FILE__, __LINE__); \
    }

namespace Rigel::Backend::Vulkan
{
    NODISCARD inline Ref<VK_Renderer> GetRenderer()
    {
        const auto& renderer = Engine::Get().GetRenderer();
        return nullptr;
    }
}
