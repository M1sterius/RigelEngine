#include "VK_MemoryBuffer.hpp"
#include "VK_CmdBuffer.hpp"
#include "VulkanUtility.hpp"
#include "Vertex.hpp"

namespace Rigel::Backend::Vulkan
{
    std::unique_ptr<VK_MemoryBuffer> VK_MemoryBuffer::MakeVertexBuffer(const std::vector<Vertex>& vertices)
    {
        auto& device = GetDevice();

        const auto bufferSize = sizeof(vertices[0]) * vertices.size();
        ASSERT(bufferSize > 0, "Vertex buffer size cannot be zero");

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, bufferSize, vertices.data());

        auto vertexBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);
        Copy(device, stagingBuffer, *vertexBuffer, bufferSize);

        return vertexBuffer;
    }

    std::unique_ptr<VK_MemoryBuffer> VK_MemoryBuffer::MakeIndexBuffer(const std::vector<uint32_t>& indices)
    {
        auto& device = GetDevice();

        const auto bufferSize = sizeof(indices[0]) * indices.size();
        ASSERT(bufferSize > 0, "Index buffer size cannot be zero");

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, bufferSize, indices.data());

        auto indexBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);
        Copy(device, stagingBuffer, *indexBuffer, bufferSize);

        return indexBuffer;
    }

    void VK_MemoryBuffer::Copy(VK_Device& device, const VK_MemoryBuffer& src, const VK_MemoryBuffer& dst, const VkDeviceSize copySize)
    {
        ASSERT(copySize <= src.GetSize(), "Copy region cannot be bigger than source buffer size");
        ASSERT(copySize <= dst.GetSize(), "Copy region cannot be bigger than destination buffer size");

        const auto cmdBuff = VK_CmdBuffer::BeginSingleTime(device, QueueType::Transfer);

        VkBufferCopy copyRegion {};
        copyRegion.size = copySize;
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
