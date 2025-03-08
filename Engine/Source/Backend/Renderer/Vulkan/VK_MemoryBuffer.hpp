#pragma once

#include "Core.hpp"
#include "vulkan.h"

namespace rge::backend
{
    class VK_Device;

    class VK_MemoryBuffer
    {
    public:
        VK_MemoryBuffer(VK_Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties);
        ~VK_MemoryBuffer();

        VK_MemoryBuffer(const VK_MemoryBuffer&) = delete;
        VK_MemoryBuffer operator = (const VK_MemoryBuffer) = delete;
    private:
        VK_Device& m_Device;
        VkDeviceSize m_Size = 0;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    };
}
