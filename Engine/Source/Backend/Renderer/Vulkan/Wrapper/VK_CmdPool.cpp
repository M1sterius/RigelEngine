#include "VK_CmdPool.hpp"
#include "MakeInfo.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    static uint32_t DeduceQueueFamilyIndex(const VK_Device& device, const QueueType type)
    {
        const auto& queueFamilyIndices = device.GetQueueFamilyIndices();

        if (type == QueueType::Graphics)
            return queueFamilyIndices.GraphicsFamily.value();
        if (type == QueueType::Transfer)
            return queueFamilyIndices.TransferFamily.value();

        Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR, "Invalid queue type!", __FILE__, __LINE__);
        return -1;
    }

    VK_CmdPool::VK_CmdPool(VK_Device& device, const QueueType queueType)
        : m_Device(device), m_QueueType(queueType)
    {
        m_QueueFamilyIndex = DeduceQueueFamilyIndex(device, queueType);

        auto poolCreateInfo = MakeInfo<VkCommandPoolCreateInfo>();
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;

        VK_CHECK_RESULT(vkCreateCommandPool(m_Device.Get(), &poolCreateInfo, nullptr, &m_CommandPool), "Failed to create command pool!");
    }

    VK_CmdPool::~VK_CmdPool()
    {
        vkDestroyCommandPool(m_Device.Get(), m_CommandPool, nullptr);
    }

    VkCommandBuffer VK_CmdPool::Allocate() const
    {
        auto allocInfo = MakeInfo<VkCommandBufferAllocateInfo>();
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device.Get(), &allocInfo, &commandBuffer), "Failed to allocate command buffer!");

        return commandBuffer;
    }

    void VK_CmdPool::Free(VkCommandBuffer commandBuffer) const
    {
        vkFreeCommandBuffers(m_Device.Get(), m_CommandPool, 1, &commandBuffer);
    }
}
