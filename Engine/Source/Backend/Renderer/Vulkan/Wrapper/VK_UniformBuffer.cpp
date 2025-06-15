#include "VK_UniformBuffer.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_Device.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_UniformBuffer::VK_UniformBuffer(VK_Device& device, const size_t size)
        : m_Device(device), m_Size(size)
    {
        ASSERT(size > 0, "Uniform buffer size cannot be zero");

        m_MemoryBuffer = std::make_unique<VK_MemoryBuffer>(m_Device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
    }

    VK_UniformBuffer::~VK_UniformBuffer() = default;

    void VK_UniformBuffer::UploadData(const size_t offset, const size_t size, const void* data) const
    {
        ASSERT(size <= m_Size, "Attempted to upload too much data into a uniform buffer");
        ASSERT(offset < m_Size, "UBO data upload offset cannot be greater or equal to the buffer size");

        m_MemoryBuffer->UploadData(offset, size, data);
    }

    void VK_UniformBuffer::UploadData(const void* data) const
    {
        UploadData(0, m_Size, data);
    }
}
