#include "VK_GBuffer.hpp"
#include "Wrapper/VK_Device.hpp"
#include "Wrapper/VK_Image.hpp"
#include "Helpers/VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GBuffer::VK_GBuffer(VK_Device& device, const glm::uvec2 size)
        : m_Device(device), m_Size(size)
    {
        Debug::Trace("Creating G-Buffer with 4 attachments.");

        Recreate(m_Size);
    }

    VK_GBuffer::~VK_GBuffer()
    {
        Debug::Trace("Destroying G-Buffer.");
    }

    void VK_GBuffer::Recreate(const glm::uvec2 size)
    {
        m_Size = size;

        if (m_Position) m_Position.reset();
        if (m_NormalRoughness) m_NormalRoughness.reset();
        if (m_AlbedoMetallic) m_AlbedoMetallic.reset();
        if (m_Depth) m_Depth.reset();

        // Position attachment (location 0)
        m_Position = std::make_unique<VK_Image>(m_Device, m_Size,
            POSITION_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Normal-roughness attachment (location 1)
        m_NormalRoughness = std::make_unique<VK_Image>(m_Device, m_Size,
            NORMAL_ROUGHNESS_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Albedo-spec attachment (location 2)
        m_AlbedoMetallic = std::make_unique<VK_Image>(m_Device, m_Size,
            ALBEDO_METALLIC_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Depth attachment
        m_Depth = std::make_unique<VK_Image>(m_Device, m_Size,
            DEPTH_STENCIL_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1);

        m_Depth->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0);

        SetupRenderingInfo();
    }

    void VK_GBuffer::CmdTransitionToRender(VkCommandBuffer commandBuffer)
    {
        VK_Image::CmdTransitionLayout(commandBuffer, m_Position->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_NormalRoughness->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_AlbedoMetallic->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);
    }

    void VK_GBuffer::CmdTransitionToSample(VkCommandBuffer commandBuffer)
    {
        VK_Image::CmdTransitionLayout(commandBuffer, m_Position->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_NormalRoughness->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_AlbedoMetallic->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);
    }

    void VK_GBuffer::SetupRenderingInfo()
    {
        // Position attachment
        m_ColorAttachments[0] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[0].imageView = m_Position->GetView();
        m_ColorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[0].clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

        // Normal attachment
        m_ColorAttachments[1] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[1].imageView = m_NormalRoughness->GetView();
        m_ColorAttachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[1].clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

        // Albedo-spec attachment
        m_ColorAttachments[2] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[2].imageView = m_AlbedoMetallic->GetView();
        m_ColorAttachments[2].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[2].clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

        // Depth attachment
        m_DepthAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        m_DepthAttachment.imageView = m_Depth->GetView();
        m_DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        m_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_DepthAttachment.clearValue.depthStencil = {1.0f, 0};

        m_RenderingInfo = MakeInfo<VkRenderingInfo>();
        m_RenderingInfo.renderArea.offset = {0, 0};
        m_RenderingInfo.renderArea.extent = { m_Size.x, m_Size.y};
        m_RenderingInfo.layerCount = 1;
        m_RenderingInfo.colorAttachmentCount = m_ColorAttachments.size();
        m_RenderingInfo.pColorAttachments = m_ColorAttachments.data();
        m_RenderingInfo.pDepthAttachment = &m_DepthAttachment;
    }
}
