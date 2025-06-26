#include "VK_Texture.hpp"
#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Device.hpp"
#include "VulkanUtility.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "TextureRegistry.hpp"
#include "Texture.hpp"
#include "BuiltInAssets.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <format>

namespace Rigel::Backend::Vulkan
{
    VK_Texture::VK_Texture(const Ref<Texture>& baseAsset)
        : m_BaseAsset(baseAsset)
    {
        const auto path = m_BaseAsset->GetPath();

        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = renderer.GetBackend();
        auto& device = backend.GetDevice();

        stbi_set_flip_vertically_on_load(true);

        // Load a texture from file into a CPU side buffer
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        const auto size = glm::uvec2(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        const VkDeviceSize imageSize = size.x * size.y * 4; // 4 bytes for 8-bit RGBA

        if (!pixels)
            throw RigelException(std::format("Failed to load a texture at path: {}!", path.string()));

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, imageSize, pixels);

        stbi_image_free(pixels);

        m_Image = std::make_unique<VK_Image>(device, size, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_Image->CopyFromBuffer(stagingBuffer);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_BindlessIndex = backend.GetTextureRegistry().AddTexture(this);
    }

    VK_Texture::~VK_Texture()
    {
        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = renderer.GetBackend();

        backend.GetTextureRegistry().RemoveTexture(m_BindlessIndex); // remove from registry
    }

    glm::uvec2 VK_Texture::GetSize() const
    {
        return m_Image->GetSize();
    }
}

