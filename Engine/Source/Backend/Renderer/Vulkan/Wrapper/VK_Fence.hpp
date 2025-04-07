#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <numeric>

namespace Rigel::Backend
{
    class VK_Device;

    class VK_Fence
    {
    public:
        explicit VK_Fence(VK_Device& device, const bool signalBit = false);
        ~VK_Fence();

        VK_Fence(const VK_Fence&) = delete;
        VK_Fence operator = (const VK_Fence&) = delete;

        NODISCARD inline VkFence Get() const { return m_Fence; }

        void Reset();
        void Wait(const uint64_t timeout = std::numeric_limits<uint64_t>::max());
    private:
        VK_Device& m_Device;
        VkFence m_Fence = VK_NULL_HANDLE;
    };
}
