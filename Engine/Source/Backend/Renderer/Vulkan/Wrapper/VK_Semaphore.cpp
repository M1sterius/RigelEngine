#include "VK_Semaphore.hpp"
#include "VK_Device.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

namespace Rigel::Backend
{
    VK_Semaphore::VK_Semaphore(VK_Device& device)
            :   m_Device(device)
    {
        auto semaphoreInfo = MakeInfo<VkSemaphoreCreateInfo>();

        if (const auto result = vkCreateSemaphore(m_Device.Get(), &semaphoreInfo, nullptr, &m_Semaphore); result != VK_SUCCESS)
            throw VulkanException("Failed to Vulkan create semaphore!", result);
    }

    VK_Semaphore::~VK_Semaphore()
    {
        vkDestroySemaphore(m_Device.Get(), m_Semaphore, nullptr);
    }
}
