#include "VK_Texture.hpp"
#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VulkanUtility.hpp"
#include "VK_BindlessManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_Texture::VK_Texture(const void* pixelData, const glm::uvec2 size)
    {
        const VkDeviceSize imageSize = size.x * size.y * 4; // 4 bytes for 8-bit RGBA

        auto& device = GetDevice();

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, imageSize, pixelData);

        const auto mipLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(size.x, size.y))) + 1);
        m_Image = std::make_unique<VK_Image>(device, size, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, mipLevelCount);

        VK_Image::TransitionLayout(*m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_Image->CopyFromBuffer(stagingBuffer);

        VK_Image::TransitionLayout(*m_Image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_BindlessIndex = GetVKRenderer().GetBindlessManager().AddTexture(this);
    }

    VK_Texture::~VK_Texture()
    {
        GetVKRenderer().GetBindlessManager().RemoveTexture(m_BindlessIndex);
    }

    glm::uvec2 VK_Texture::GetSize() const
    {
        return m_Image->GetSize();
    }
}

