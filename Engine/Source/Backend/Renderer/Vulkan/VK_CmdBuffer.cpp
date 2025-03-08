#include "VK_CmdBuffer.hpp"
#include "VK_Device.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

namespace rge::backend
{
    VK_CmdBuffer::VK_CmdBuffer(VK_Device& device)
        : m_Device(device)
    {
        // TODO: Implement allocating command buffers from pool created per thread or per queue family

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

    void VK_CmdBuffer::BeginRecording(VkCommandBufferUsageFlags flags)
    {
        auto beginInfo = MakeInfo<VkCommandBufferBeginInfo>();
        beginInfo.flags = flags;

        if (const auto result = vkBeginCommandBuffer(m_CmdBuffer, &beginInfo); result != VK_SUCCESS)
            throw VulkanException("Failed to begin recording Vulkan command buffer!", result);
    }

    void VK_CmdBuffer::EndRecording()
    {
        if (const auto result = vkEndCommandBuffer(m_CmdBuffer); result != VK_SUCCESS)
            throw VulkanException("Failed to end recording Vulkan command buffer!", result);
    }

    void VK_CmdBuffer::Reset(VkCommandBufferResetFlags flags)
    {
        if (const auto result = vkResetCommandBuffer(m_CmdBuffer, flags); result != VK_SUCCESS)
            throw VulkanException("Failed to reset Vulkan command buffer!", result);
    }
}
