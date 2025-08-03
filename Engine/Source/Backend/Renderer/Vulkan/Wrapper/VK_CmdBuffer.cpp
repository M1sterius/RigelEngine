#include "VK_CmdBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_Fence.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    std::unique_ptr<VK_CmdBuffer> VK_CmdBuffer::BeginSingleTime(VK_Device& device, const QueueType queueType)
    {
        auto cmdBuffer = std::make_unique<VK_CmdBuffer>(device, queueType);
        cmdBuffer->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        return cmdBuffer;
    }

    void VK_CmdBuffer::EndSingleTime(const VK_Device& device, const VK_CmdBuffer& commandBuffer)
    {
        commandBuffer.EndRecording();

        auto submitInfo = MakeInfo<VkSubmitInfo>();
        const auto buff = commandBuffer.Get();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buff;

        // Using a fence helps to avoid stalling the entire queue (so don't use vkQueueWaitIdle)
        const auto fence = std::make_unique<VK_Fence>(device);
        device.SubmitToQueue(commandBuffer.m_QueueType, 1, &submitInfo, fence->Get());
        fence->Wait();
    }

    VK_CmdBuffer::VK_CmdBuffer(VK_Device& device, const QueueType queueType)
        : m_Device(device), m_QueueType(queueType)
    {
        auto allocInfo = MakeInfo<VkCommandBufferAllocateInfo>();
        allocInfo.commandPool = m_Device.GetCommandPool(queueType);
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device.Get(), &allocInfo, &m_CmdBuffer), "Failed to allocate command buffer!");
    }

    VK_CmdBuffer::~VK_CmdBuffer()
    {
        vkFreeCommandBuffers(m_Device.Get(), m_Device.GetCommandPool(m_QueueType), 1, &m_CmdBuffer);
    }

    void VK_CmdBuffer::BeginRecording(VkCommandBufferUsageFlags flags) const
    {
        auto beginInfo = MakeInfo<VkCommandBufferBeginInfo>();
        beginInfo.flags = flags;

        VK_CHECK_RESULT(vkBeginCommandBuffer(m_CmdBuffer, &beginInfo), "Failed to begin recording command buffer!");
    }

    void VK_CmdBuffer::EndRecording() const
    {
        VK_CHECK_RESULT(vkEndCommandBuffer(m_CmdBuffer), "Failed to end recording command buffer!");
    }

    void VK_CmdBuffer::Reset(VkCommandBufferResetFlags flags) const
    {
        VK_CHECK_RESULT(vkResetCommandBuffer(m_CmdBuffer, flags), "Failed to reset command buffer!");
    }
}
