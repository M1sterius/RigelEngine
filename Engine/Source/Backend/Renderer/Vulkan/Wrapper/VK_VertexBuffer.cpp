#include "VK_VertexBuffer.hpp"
#include "Vertex.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_VertexBuffer::VK_VertexBuffer(const std::vector<Vertex>& vertices)
        : m_VertexCount(vertices.size())
    {
        ASSERT(m_VertexCount > 0, "Vertex buffer size cannot be zero");

        const auto& renderer = GetVKRenderer();
        auto& device = renderer.GetDevice();

        const auto bufferSize = sizeof(vertices[0]) * vertices.size();

        auto& stagingBuffer = renderer.GetStagingBuffer();
        stagingBuffer.UploadData(0, bufferSize, vertices.data());

        m_Buffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);
        VK_MemoryBuffer::Copy(device, stagingBuffer, *m_Buffer, bufferSize);
    }

    VK_VertexBuffer::~VK_VertexBuffer() = default;

    void VK_VertexBuffer::CmdBind(VkCommandBuffer commandBuffer) const
    {
        const VkBuffer vertexBuffers[] = {m_Buffer->Get()};
        constexpr VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }
}
