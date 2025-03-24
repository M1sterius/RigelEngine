#include "IndexBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_MemoryBuffer.hpp"

namespace rge::backend
{
    VK_IndexBuffer::VK_IndexBuffer(VK_Device& device, const std::vector<uint32_t>& indices)
    : m_IndexCount(indices.size())
    {
        const auto bufferSize = sizeof(indices[0]) * indices.size();
        ASSERT(bufferSize > 0, "Index buffer size cannot be zero");

        const auto stagingBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer->UploadData(0, 0, bufferSize, indices.data());

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_MemoryBuffer::Copy(device, *stagingBuffer, *m_MemoryBuffer, bufferSize);
    }

    VK_IndexBuffer::~VK_IndexBuffer() = default;
}