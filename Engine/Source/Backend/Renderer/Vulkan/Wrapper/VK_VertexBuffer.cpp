#include "VK_VertexBuffer.hpp"
#include "Vertex.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_StagingManager.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_VertexBuffer::VK_VertexBuffer(const size_t vertexSize, const size_t vertexCount, const void* vertexData)
        : m_VertexCount(vertexCount)
    {
        ASSERT(vertexSize > 0, "Vertex struct size cannot be zero!")
        ASSERT(m_VertexCount > 0, "Vertex buffer size cannot be zero");
        ASSERT(vertexData, "vertexData was a nullptr!");

        const auto& renderer = GetVKRenderer();
        auto& device = renderer.GetDevice();

        const auto bufferSize = vertexSize * vertexCount;

        auto& stagingBuffer = renderer.GetStagingManager().GetBuffer();
        stagingBuffer.UploadData(0, bufferSize, vertexData);

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
