#pragma once

#include "Core.hpp"
#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_Semaphore
    {
    public:
        explicit VK_Semaphore(VK_Device& device);
        ~VK_Semaphore();

        VK_Semaphore(const VK_Semaphore&) = delete;
        VK_Semaphore operator = (const VK_Semaphore&) = delete;

        NODISCARD inline VkSemaphore Get() const { return m_Semaphore; }
    private:
        VK_Device& m_Device;
        VkSemaphore m_Semaphore = VK_NULL_HANDLE;
    };
}
