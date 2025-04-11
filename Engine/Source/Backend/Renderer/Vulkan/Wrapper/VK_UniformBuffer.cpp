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
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        m_MapPtr = m_MemoryBuffer->Map(0, 0, m_Size);
    }

    VK_UniformBuffer::~VK_UniformBuffer() = default;

    void VK_UniformBuffer::UploadData(const size_t offset, const size_t size, const void* data) const
    {
        ASSERT(size <= m_Size, "Attempted to upload too much data into a uniform buffer");
        ASSERT(offset < m_Size, "UBO data upload offset cannot be greater or equal to the buffer size");

        const auto cpyAddr = static_cast<uint8_t*>(m_MapPtr) + offset;
        memcpy(cpyAddr, data, size);
    }

    void VK_UniformBuffer::UploadData(const void* data) const
    {
        UploadData(0, m_Size, data);
    }
}
