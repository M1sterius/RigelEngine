#include "VK_Texture.hpp"
#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "TextureRegistry.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_Texture::VK_Texture(const void* pixelData, const glm::uvec2 size)
    {
        const VkDeviceSize imageSize = size.x * size.y * 4; // 4 bytes for 8-bit RGBA

        auto& device = GetDevice();

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, imageSize, pixelData);

        m_Image = std::make_unique<VK_Image>(device, size, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_Image->CopyFromBuffer(stagingBuffer);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_BindlessIndex = GetRenderer().GetTextureRegistry().AddTexture(this);
    }

    VK_Texture::~VK_Texture()
    {
        GetRenderer().GetTextureRegistry().RemoveTexture(m_BindlessIndex); // remove from registry
    }

    glm::uvec2 VK_Texture::GetSize() const
    {
        return m_Image->GetSize();
    }
}

