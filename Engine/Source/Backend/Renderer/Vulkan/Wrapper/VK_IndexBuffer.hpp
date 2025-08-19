#pragma once

#include "Core.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    class VK_MemoryBuffer;

    class VK_IndexBuffer
    {
    public:
        explicit VK_IndexBuffer(const std::vector<uint32_t>& indices);
        ~VK_IndexBuffer();

        VK_IndexBuffer(const VK_IndexBuffer&) = delete;
        VK_IndexBuffer operator = (const VK_IndexBuffer&) = delete;

        void CmdBind(VkCommandBuffer commandBuffer) const;
    private:
        uint32_t m_IndexCount;
        std::unique_ptr<VK_MemoryBuffer> m_Buffer;
    };
}
