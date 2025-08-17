#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include <memory>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    struct Vertex;

    class VK_MemoryBuffer
    {
    public:
        static void Copy(VK_Device& device, const VK_MemoryBuffer& src, const VK_MemoryBuffer& dst, const VkDeviceSize copySize);

        inline static bool EnableAutoResizeOnUpload = true;

        static std::unique_ptr<VK_MemoryBuffer> MakeVertexBuffer(const std::vector<Vertex>& vertices);
        static std::unique_ptr<VK_MemoryBuffer> MakeIndexBuffer(const std::vector<uint32_t>& indices);

        VK_MemoryBuffer(VK_Device& device, VkDeviceSize size, VkBufferUsageFlags buffUsage,
            VmaMemoryUsage memUsage);
        ~VK_MemoryBuffer();

        VK_MemoryBuffer(const VK_MemoryBuffer&) = delete;
        VK_MemoryBuffer operator = (const VK_MemoryBuffer) = delete;

        NODISCARD inline VkBuffer Get() const { return m_Buffer; }
        NODISCARD inline VkDeviceSize GetSize() const { return m_Size; }

        // WARNING: All data in the buffer will be lost after resizing!
        void Resize(const VkDeviceSize newSize);
        void UploadData(const VkDeviceSize offset, const VkDeviceSize size, const void* data);
    private:
        VK_Device& m_Device;
        VkDeviceSize m_Size = 0;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VkBufferUsageFlags m_BufferUsage;
        VmaMemoryUsage m_MemoryUsage;

        void CreateBuffer(const VkDeviceSize size);
    };
}
