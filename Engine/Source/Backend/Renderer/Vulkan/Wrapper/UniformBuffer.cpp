#include "UniformBuffer.hpp"

#include <VK_MemoryBuffer.hpp>

namespace rge::backend
{
    VK_UniformBuffer::VK_UniformBuffer(VK_Device& device, const size_t size)
        : m_Device(device), m_Size(size)
    {
        ASSERT(size > 0, "Uniform buffer size cannot be zero");

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(m_Device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        m_MapPtr = m_MemoryBuffer->Map(0, 0, m_Size);
    }

    VK_UniformBuffer::~VK_UniformBuffer() = default;

    void VK_UniformBuffer::UploadData(const size_t offset, const size_t size, const void* data) const
    {
        memcpy(m_MapPtr + offset, data, size);
    }

    void VK_UniformBuffer::UploadData(const void* data) const
    {
        UploadData(0, m_Size, data);
    }
}
