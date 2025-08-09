#include "VK_MemoryBuffer.hpp"
#include "VK_CmdBuffer.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    void VK_MemoryBuffer::Copy(VK_Device& device, const VK_MemoryBuffer& src, const VK_MemoryBuffer& dst, const VkDeviceSize size)
    {
        ASSERT(size <= src.GetSize(), "Copy region cannot be bigger than source buffer size");
        ASSERT(size <= dst.GetSize(), "Copy region cannot be bigger than destination buffer size");

        const auto cmdBuff = VK_CmdBuffer::BeginSingleTime(device, QueueType::Transfer);

        VkBufferCopy copyRegion {};
        copyRegion.size = size;
        vkCmdCopyBuffer(cmdBuff->Get(), src.Get(), dst.Get(), 1, &copyRegion);

        VK_CmdBuffer::EndSingleTime(cmdBuff);
    }

    VK_MemoryBuffer::VK_MemoryBuffer(VK_Device& device, VkDeviceSize size, VkBufferUsageFlags buffUsage, VmaMemoryUsage memUsage)
        : m_Device(device), m_Size(size), m_BufferUsage(buffUsage), m_MemoryUsage(memUsage)
    {
        CreateBuffer();
    }

    VK_MemoryBuffer::~VK_MemoryBuffer()
    {
        vmaDestroyBuffer(m_Device.GetVmaAllocator(), m_Buffer, m_Allocation);
    }

    void VK_MemoryBuffer::CreateBuffer()
    {
        ASSERT(m_Size > 0, "Vulkan buffer size must be greater than zero!");

        auto bufferInfo = MakeInfo<VkBufferCreateInfo>();
        bufferInfo.size = m_Size;
        bufferInfo.usage = m_BufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = m_MemoryUsage;

        VK_CHECK_RESULT(vmaCreateBuffer(m_Device.GetVmaAllocator(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create vma memory buffer!");
    }

    void VK_MemoryBuffer::Resize(const VkDeviceSize newSize)
    {
        ASSERT(m_Size > 0, "Vulkan buffer size must be greater than zero!");

        vmaDestroyBuffer(m_Device.GetVmaAllocator(), m_Buffer, m_Allocation);

        m_Size = newSize;
        CreateBuffer();
    }

    void VK_MemoryBuffer::UploadData(const VkDeviceSize offset, const VkDeviceSize size, const void* data)
    {
        ASSERT(size > 0, "Attempted to upload data of zero size!")

        if (EnableAutoResizeOnUpload)
        {
            if (offset + size > m_Size)
                Resize(offset + size);
        }

        VK_CHECK_RESULT(vmaCopyMemoryToAllocation(m_Device.GetVmaAllocator(), data, m_Allocation, offset, size), "Failed to copy data to vma allocation!");
    }
}
