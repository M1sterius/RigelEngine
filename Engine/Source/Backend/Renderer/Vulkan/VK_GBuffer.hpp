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
    class VK_DescriptorPool;

    class VK_GBuffer
    {
    public:
        static constexpr VkFormat POSITION_ATTACHMENT_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
        static constexpr VkFormat NORMAL_ROUGHNESS_ATTACHMENT_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
        static constexpr VkFormat ALBEDO_METALLIC_ATTACHMENT_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
        static constexpr VkFormat DEPTH_STENCIL_ATTACHMENT_FORMAT = VK_FORMAT_D32_SFLOAT_S8_UINT;

        VK_GBuffer(VK_Device& device, const glm::uvec2 size);
        ~VK_GBuffer();

        VK_GBuffer(const VK_GBuffer&) = delete;
        VK_GBuffer operator = (const VK_GBuffer&) = delete;

        void Recreate(const glm::uvec2 size);

        void CmdTransitionToRender(VkCommandBuffer commandBuffer);
        void CmdTransitionToSample(VkCommandBuffer commandBuffer);

        NODISCARD VkRenderingInfo* GetRenderingInfo() { return &m_RenderingInfo; }

        NODISCARD Ref<VK_Image> GetPosition() const { return m_Position.get(); }
        NODISCARD Ref<VK_Image> GetNormalRoughness() const { return m_NormalRoughness.get(); }
        NODISCARD Ref<VK_Image> GetAlbedoMetallic() const { return m_AlbedoMetallic.get(); }
        NODISCARD Ref<VK_Image> GetDepth() const { return m_Depth.get(); }
    private:
        VK_Device& m_Device;
        glm::uvec2 m_Size;

        std::unique_ptr<VK_Image> m_Position;
        std::unique_ptr<VK_Image> m_NormalRoughness;
        std::unique_ptr<VK_Image> m_AlbedoMetallic;
        std::unique_ptr<VK_Image> m_Depth;

        VkRenderingInfo m_RenderingInfo{};
        VkRenderingAttachmentInfo m_DepthAttachment{};
        std::array<VkRenderingAttachmentInfo, 3> m_ColorAttachments{};

        void SetupRenderingInfo();
    };
}
