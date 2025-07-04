#include "VK_Semaphore.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_Semaphore::VK_Semaphore(VK_Device& device)
        : m_Device(device)
    {
        auto semaphoreInfo = MakeInfo<VkSemaphoreCreateInfo>();

        VK_CHECK_RESULT(vkCreateSemaphore(m_Device.Get(), &semaphoreInfo, nullptr, &m_Semaphore), "Failed to create semaphore!");
    }

    VK_Semaphore::~VK_Semaphore()
    {
        vkDestroySemaphore(m_Device.Get(), m_Semaphore, nullptr);
    }
}
