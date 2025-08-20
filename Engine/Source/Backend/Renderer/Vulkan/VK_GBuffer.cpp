#include "VK_GBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_Image.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GBuffer::VK_GBuffer(VK_Device& device, const glm::uvec2 size)
        : m_Device(device), m_Size(size)
    {
        Setup(m_Size);
    }

    VK_GBuffer::~VK_GBuffer() = default;

    void VK_GBuffer::Setup(const glm::uvec2 size)
    {
        m_Size = size;

        if (m_Position) m_Position.reset();
        if (m_Normal) m_Normal.reset();
        if (m_AlbedoSpec) m_AlbedoSpec.reset();
        if (m_Depth) m_Depth.reset();

        // Position attachment (location 0)
        m_Position = std::make_unique<VK_Image>(m_Device, m_Size,
            VK_FORMAT_R32G32B32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Normal attachment (location 1)
        m_Normal = std::make_unique<VK_Image>(m_Device, m_Size,
            VK_FORMAT_R32G32B32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Albedo-spec attachment (location 2)
        m_AlbedoSpec = std::make_unique<VK_Image>(m_Device, m_Size,
            VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Depth attachment
        m_Depth = std::make_unique<VK_Image>(m_Device, m_Size,
            VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    }
}
