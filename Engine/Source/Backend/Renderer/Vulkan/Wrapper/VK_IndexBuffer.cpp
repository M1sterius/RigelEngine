#include "VK_IndexBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_MemoryBuffer.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_IndexBuffer::VK_IndexBuffer(VK_Device& device, const std::vector<uint32_t>& indices)
    : m_IndexCount(indices.size())
    {
        const auto bufferSize = sizeof(indices[0]) * indices.size();
        ASSERT(bufferSize > 0, "Index buffer size cannot be zero");

        const auto stagingBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
        stagingBuffer->UploadData(0, bufferSize, indices.data());

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VMA_MEMORY_USAGE_GPU_ONLY);

        VK_MemoryBuffer::Copy(device, *stagingBuffer, *m_MemoryBuffer, bufferSize);
    }

    VK_IndexBuffer::~VK_IndexBuffer() = default;
}