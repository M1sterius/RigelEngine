#pragma once

#include "Core.hpp"
#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_CmdBuffer
    {
    public:
        NODISCARD static std::unique_ptr<VK_CmdBuffer> BeginSingleTime(VK_Device& device);
        static void EndSingleTime(const VK_Device& device, const VK_CmdBuffer& commandBuffer);

        static void CmdSetScissor(VkCommandBuffer cmdBuffer, const glm::ivec2 offset, const VkExtent2D extent);
        static void CmdSetViewport(VkCommandBuffer cmdBuffer, const glm::vec2 offset, const glm::vec2 size,
            const glm::vec2 depthBounds);

        explicit VK_CmdBuffer(VK_Device& device);
        ~VK_CmdBuffer();

        VK_CmdBuffer(const VK_CmdBuffer&) = delete;
        VK_CmdBuffer operator = (const VK_CmdBuffer&) = delete;

        NODISCARD inline VkCommandBuffer Get() const { return m_CmdBuffer; }

        void BeginRecording(VkCommandBufferUsageFlags flags) const;
        void EndRecording() const;
        void Reset(VkCommandBufferResetFlags flags) const;
    private:
        VK_Device& m_Device;
        VkCommandBuffer m_CmdBuffer = VK_NULL_HANDLE;
    };
}
