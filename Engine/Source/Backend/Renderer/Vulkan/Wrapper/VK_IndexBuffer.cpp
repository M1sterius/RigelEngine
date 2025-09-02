#include "VK_IndexBuffer.hpp"
#include "VK_MemoryBuffer.hpp"
#include "../VK_StagingManager.hpp"
#include "../Helpers/VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_IndexBuffer::VK_IndexBuffer(const std::vector<uint32_t>& indices)
        : m_IndexCount(indices.size())
    {
        ASSERT(m_IndexCount > 0, "Index buffer size cannot be zero");

        const auto& renderer = GetVKRenderer();
        auto& device = renderer.GetDevice();

        const auto bufferSize = sizeof(indices[0]) * indices.size();

        auto& stagingBuffer = renderer.GetStagingManager().GetBuffer();
        stagingBuffer.UploadData(0, bufferSize, indices.data());

        m_Buffer = std::make_unique<VK_MemoryBuffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);
        VK_MemoryBuffer::Copy(device, stagingBuffer, *m_Buffer, bufferSize);
    }

    VK_IndexBuffer::~VK_IndexBuffer() = default;

    void VK_IndexBuffer::CmdBind(VkCommandBuffer commandBuffer) const
    {
        vkCmdBindIndexBuffer(commandBuffer, m_Buffer->Get(), 0, VK_INDEX_TYPE_UINT32);
    }
}
