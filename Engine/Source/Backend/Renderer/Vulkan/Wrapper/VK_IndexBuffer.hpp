#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <vector>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_MemoryBuffer;

    class VK_IndexBuffer
    {
    public:
        VK_IndexBuffer(VK_Device& device, const std::vector<uint32_t>& indices);
        ~VK_IndexBuffer();

        VK_IndexBuffer(const VK_IndexBuffer&) = delete;
        VK_IndexBuffer operator = (const VK_IndexBuffer&) = delete;

        NODISCARD VK_MemoryBuffer& GetMemoryBuffer() const { return *m_MemoryBuffer; }
        NODISCARD uint32_t GetIndexCount() const { return m_IndexCount; }
    private:
        std::unique_ptr<VK_MemoryBuffer> m_MemoryBuffer;
        uint32_t m_IndexCount;
    };

}
