#include "VK_MemoryBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_CmdBuffer.hpp"
#include "VulkanException.hpp"
#include "MakeInfo.hpp"

namespace Rigel::Backend::Vulkan
{
    void VK_MemoryBuffer::Copy(VK_Device& device, const VK_MemoryBuffer& src, const VK_MemoryBuffer& dst, const VkDeviceSize size)
    {
        ASSERT(size <= src.GetSize(), "Copy region cannot be bigger than source buffer size");
        ASSERT(size <= dst.GetSize(), "Copy region cannot be bigger than destination buffer size");

        const auto commandBuffer = VK_CmdBuffer::BeginSingleTime(device);

        VkBufferCopy copyRegion {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer->Get(), src.Get(), dst.Get(), 1, &copyRegion);

        VK_CmdBuffer::EndSingleTime(device, *commandBuffer);
    }

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

    void* VK_MemoryBuffer::Map(const VkDeviceSize offset, const VkMemoryMapFlags flags, const VkDeviceSize size) const
    {
        void* map;

        if (const auto result = vkMapMemory(m_Device.Get(), m_BufferMemory, offset, m_Size, flags, &map); result != VK_SUCCESS)
            throw VulkanException("Failed to map Vulkan GPU buffer memory to host memory!", result);

        return map;
    }

    void VK_MemoryBuffer::UploadData(const VkDeviceSize offset, const VkMemoryMapFlags flags, const VkDeviceSize size, const void* data) const
    {
        ASSERT(size > 0, "Attempted to upload data of zero size!")
        ASSERT(size <= m_Size, "Attempted to upload more data than the buffer is capable of storing!")

        void* map;

        if (const auto result = vkMapMemory(m_Device.Get(), m_BufferMemory, offset, m_Size, flags, &map); result != VK_SUCCESS)
            throw VulkanException("Failed to map Vulkan GPU buffer memory to host memory!", result);

        memcpy(map, data, m_Size);
        vkUnmapMemory(m_Device.Get(), m_BufferMemory);
    }
}
