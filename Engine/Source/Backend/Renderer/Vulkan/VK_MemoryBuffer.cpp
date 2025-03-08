#include "VK_MemoryBuffer.hpp"
#include "VK_Device.hpp"
#include "VulkanException.hpp"
#include "MakeInfo.hpp"

namespace rge::backend
{

    VK_MemoryBuffer::VK_MemoryBuffer(VK_Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
                                     VkMemoryPropertyFlags properties)
        : m_Device(device), m_Size(size)
    {
        ASSERT(size > 0, "Vulkan buffer size must be greater than zero!");

        auto bufferInfo = MakeInfo<VkBufferCreateInfo>();
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (const auto result = vkCreateBuffer(m_Device.Get(), &bufferInfo, nullptr, &m_Buffer); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan GPU memory buffer!", result);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device.Get(), m_Buffer, &memRequirements);

        auto allocInfo = MakeInfo<VkMemoryAllocateInfo>();
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (const auto result = vkAllocateMemory(m_Device.Get(), &allocInfo, nullptr, &m_BufferMemory); result != VK_SUCCESS)
            throw VulkanException("Failed to allocate Vulkan memory for a GPU buffer!", result);

        if (const auto result = vkBindBufferMemory(m_Device.Get(), m_Buffer, m_BufferMemory, 0); result != VK_SUCCESS)
            throw VulkanException("Failed to bind allocated memory to Vulkan GPU buffer!", result);
    }

    VK_MemoryBuffer::~VK_MemoryBuffer()
    {
        vkDestroyBuffer(m_Device.Get(), m_Buffer, nullptr);
        vkFreeMemory(m_Device.Get(), m_BufferMemory, nullptr);
    }
}
