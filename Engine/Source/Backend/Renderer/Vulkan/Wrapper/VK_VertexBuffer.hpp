#pragma once

#include "Core.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    struct Vertex3p2t3n;
    class VK_MemoryBuffer;

    class VK_VertexBuffer
    {
    public:
        template<typename vertexType>
        explicit VK_VertexBuffer(const std::vector<vertexType>& vertices)
            : VK_VertexBuffer(sizeof(vertexType), vertices.size(), vertices.data()) { }

        VK_VertexBuffer(const size_t vertexSize, const size_t vertexCount, const void* vertexData);
        ~VK_VertexBuffer();

        VK_VertexBuffer(const VK_VertexBuffer&) = delete;
        VK_VertexBuffer operator = (const VK_VertexBuffer&) = delete;

        void CmdBind(VkCommandBuffer commandBuffer) const;
    private:
        uint32_t m_VertexCount;
        std::unique_ptr<VK_MemoryBuffer> m_Buffer;
    };
}
