#include "VK_VertexBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_MemoryBuffer.hpp"

namespace rge::backend
{
    VK_VertexBuffer::VK_VertexBuffer(VK_Device& device, const std::vector<Vertex>& vertices)
        : m_VertexCount(vertices.size())
    {
        const auto bufferSize = sizeof(vertices[0]) * vertices.size();
        ASSERT(bufferSize > 0, "Vertex buffer size cannot be zero");

        const auto stagingBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer->UploadData(0, 0, bufferSize, vertices.data());

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_MemoryBuffer::Copy(device, *stagingBuffer, *m_MemoryBuffer, bufferSize);
    }

    VK_VertexBuffer::~VK_VertexBuffer() = default;
}
