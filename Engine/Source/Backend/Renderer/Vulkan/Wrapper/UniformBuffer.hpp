#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <memory>

namespace rge::backend
{
    class VK_Device;
    class VK_MemoryBuffer;

    class VK_UniformBuffer
    {
    public:
        VK_UniformBuffer(VK_Device& device, const size_t size);
        ~VK_UniformBuffer();

        NODISCARD VK_MemoryBuffer& GetMemoryBuffer() const { return *m_MemoryBuffer; }

        void UploadData(const size_t offset, const size_t size, const void* data) const;
        void UploadData(const void* data) const;
    private:
        VK_Device& m_Device;

        std::unique_ptr<VK_MemoryBuffer> m_MemoryBuffer;
        const size_t m_Size;
        void* m_MapPtr = nullptr;
    };
}
