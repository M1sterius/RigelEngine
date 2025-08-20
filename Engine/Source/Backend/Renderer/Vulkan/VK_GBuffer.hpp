#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <array>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Image;

    class VK_GBuffer
    {
    public:
        VK_GBuffer(VK_Device& device, const glm::uvec2 size);
        ~VK_GBuffer();

        VK_GBuffer(const VK_GBuffer&) = delete;
        VK_GBuffer operator = (const VK_GBuffer&) = delete;

        void Setup(const glm::uvec2 size);

        void CmdTransitionToRender(VkCommandBuffer commandBuffer);
        void CmdTransitionToSample(VkCommandBuffer commandBuffer);

        NODISCARD inline VkRenderingInfo* GetRenderingInfo() { return &m_RenderingInfo; }
    private:
        VK_Device& m_Device;
        glm::uvec2 m_Size;

        std::unique_ptr<VK_Image> m_Position;
        std::unique_ptr<VK_Image> m_Normal;
        std::unique_ptr<VK_Image> m_AlbedoSpec;
        std::unique_ptr<VK_Image> m_Depth;

        std::array<VkRenderingAttachmentInfo, 3> m_ColorAttachments{};
        VkRenderingAttachmentInfo m_DepthAttachment{};
        VkRenderingInfo m_RenderingInfo{};

        void SetupRenderingInfo();
    };
}
