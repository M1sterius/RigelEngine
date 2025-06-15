#include "VK_VertexBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_MemoryBuffer.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_VertexBuffer::VK_VertexBuffer(VK_Device& device, const std::vector<Vertex>& vertices)
        : m_VertexCount(vertices.size())
    {
        const auto bufferSize = sizeof(vertices[0]) * vertices.size();
        ASSERT(bufferSize > 0, "Vertex buffer size cannot be zero");

        const auto stagingBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
        stagingBuffer->UploadData(0, bufferSize, vertices.data());

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);

        VK_MemoryBuffer::Copy(device, *stagingBuffer, *m_MemoryBuffer, bufferSize);
    }

    VK_VertexBuffer::~VK_VertexBuffer() = default;
}
