#pragma once

#include "Core.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    struct Vertex;
    class VK_MemoryBuffer;

    class VK_VertexBuffer
    {
    public:
        explicit VK_VertexBuffer(const std::vector<Vertex>& vertices);
        ~VK_VertexBuffer();

        VK_VertexBuffer(const VK_VertexBuffer&) = delete;
        VK_VertexBuffer operator = (const VK_VertexBuffer&) = delete;

        void CmdBind(VkCommandBuffer commandBuffer) const;
    private:
        uint32_t m_VertexCount;
        std::unique_ptr<VK_MemoryBuffer> m_Buffer;
    };
}
