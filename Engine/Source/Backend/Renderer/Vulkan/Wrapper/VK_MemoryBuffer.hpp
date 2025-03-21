#pragma once

#include "Core.hpp"
#include "vulkan.h"

namespace rge::backend
{
    class VK_Device;

    class VK_MemoryBuffer
    {
    public:
        static void Copy(VK_Device& device, VK_MemoryBuffer& src, VK_MemoryBuffer& dst, const VkDeviceSize size);

        VK_MemoryBuffer(VK_Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties);
        ~VK_MemoryBuffer();

        VK_MemoryBuffer(const VK_MemoryBuffer&) = delete;
        VK_MemoryBuffer operator = (const VK_MemoryBuffer) = delete;

        NODISCARD inline VkBuffer Get() const { return m_Buffer; }
        NODISCARD inline VkDeviceMemory GetMemory() const { return m_BufferMemory; }
        NODISCARD inline VkDeviceSize GetSize() const { return m_Size; }

        void UploadData(const VkDeviceSize offset, const VkMemoryMapFlags flags, const VkDeviceSize size, const void* data) const;
    private:
        VK_Device& m_Device;
        VkDeviceSize m_Size = 0;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    };
}
