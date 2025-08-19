#include "VK_CmdPool.hpp"
#include "MakeInfo.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    static uint32_t DeduceQueueFamilyIndex(const VK_Device& device, const QueueType type)
    {
        switch (type)
        {
        case QueueType::Graphics:
            return device.GetQueueFamilyIndices().GraphicsFamily.value();
        case QueueType::Transfer:
            return device.GetQueueFamilyIndices().TransferFamily.value();
        default:
            {
                Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, "Invalid queue type!", __FILE__, __LINE__);
            }
        }

        return -1;
    }

    VK_CmdPool::VK_CmdPool(VK_Device& device, const QueueType queueType)
        : m_Device(device), m_QueueType(queueType)
    {
        auto poolCreateInfo = MakeInfo<VkCommandPoolCreateInfo>();
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = DeduceQueueFamilyIndex(m_Device, queueType);

        VK_CHECK_RESULT(vkCreateCommandPool(m_Device.Get(), &poolCreateInfo, nullptr, &m_CommandPool), "Failed to create command pool!");
    }

    VK_CmdPool::~VK_CmdPool()
    {
        vkDestroyCommandPool(m_Device.Get(), m_CommandPool, nullptr);
    }
}
