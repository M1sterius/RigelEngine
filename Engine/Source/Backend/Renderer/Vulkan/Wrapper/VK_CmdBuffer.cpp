#include "VK_CmdBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_Fence.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

namespace Rigel::Backend::Vulkan
{
    std::unique_ptr<VK_CmdBuffer> VK_CmdBuffer::BeginSingleTime(VK_Device& device)
    {
        auto cmdBuffer = std::make_unique<VK_CmdBuffer>(device);
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

        // This avoids stalling the entire queue (so don't use vkQueueWaitIdle)
        const auto fence = std::make_unique<VK_Fence>(device);
        device.SubmitGraphicsQueue(1, &submitInfo, fence->Get());
        fence->Wait();
    }

    void VK_CmdBuffer::CmdSetScissor(VkCommandBuffer cmdBuffer, const glm::ivec2 offset, const VkExtent2D extent)
    {
        const VkRect2D scissor {
            .offset = {offset.x, offset.y},
            .extent = extent
        };

        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
    }

    void VK_CmdBuffer::CmdSetViewport(VkCommandBuffer cmdBuffer, const glm::vec2 offset, const glm::vec2 size,
        const glm::vec2 depthBounds)
    {
        const VkViewport viewport {
            .x = offset.x,
            .y = offset.y,
            .width = size.x,
            .height = size.y,
            .minDepth = depthBounds.x,
            .maxDepth = depthBounds.y
        };

        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
    }

    VK_CmdBuffer::VK_CmdBuffer(VK_Device& device)
        : m_Device(device)
    {
        auto allocInfo = MakeInfo<VkCommandBufferAllocateInfo>();
        allocInfo.commandPool = m_Device.GetCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (const auto result = vkAllocateCommandBuffers(m_Device.Get(), &allocInfo, &m_CmdBuffer); result != VK_SUCCESS)
            throw VulkanException("Failed to allocate vulkan command buffer!", result);
    }

    VK_CmdBuffer::~VK_CmdBuffer()
    {
        vkFreeCommandBuffers(m_Device.Get(), m_Device.GetCommandPool(), 1, &m_CmdBuffer);
    }

    void VK_CmdBuffer::BeginRecording(VkCommandBufferUsageFlags flags) const
    {
        auto beginInfo = MakeInfo<VkCommandBufferBeginInfo>();
        beginInfo.flags = flags;

        if (const auto result = vkBeginCommandBuffer(m_CmdBuffer, &beginInfo); result != VK_SUCCESS)
            throw VulkanException("Failed to begin recording Vulkan command buffer!", result);
    }

    void VK_CmdBuffer::EndRecording() const
    {
        if (const auto result = vkEndCommandBuffer(m_CmdBuffer); result != VK_SUCCESS)
            throw VulkanException("Failed to end recording Vulkan command buffer!", result);
    }

    void VK_CmdBuffer::Reset(VkCommandBufferResetFlags flags) const
    {
        if (const auto result = vkResetCommandBuffer(m_CmdBuffer, flags); result != VK_SUCCESS)
            throw VulkanException("Failed to reset Vulkan command buffer!", result);
    }
}
