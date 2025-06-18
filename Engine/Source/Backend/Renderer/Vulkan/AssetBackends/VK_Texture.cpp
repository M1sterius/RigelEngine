#include "VK_Texture.hpp"
#include "VK_Image.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VK_Renderer.hpp"
#include "VK_Device.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "TextureRegistry.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <format>

namespace Rigel::Backend::Vulkan
{
    VK_Texture::VK_Texture(std::filesystem::path path) : m_Path(std::move(path))
    {
        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = renderer.GetBackend();
        auto& device = backend.GetDevice();

        stbi_set_flip_vertically_on_load(true);

        // Load a texture from file into a CPU side buffer
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(m_Path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        m_Size.x = static_cast<uint32_t>(texWidth);
        m_Size.y = static_cast<uint32_t>(texHeight);

        const VkDeviceSize imageSize = m_Size.x * m_Size.y * 4; // 4 bytes for 8-bit RGBA

        if (!pixels)
            throw RigelException(std::format("Failed to load a texture at path: {}!", m_Path.string()));

        auto& stagingBuffer = device.GetStagingBuffer();
        stagingBuffer.UploadData(0, imageSize, pixels);

        stbi_image_free(pixels);

        m_Image = std::make_unique<VK_Image>(device, m_Size, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        m_Image->CopyFromBuffer(stagingBuffer);

        VK_Image::TransitionLayout(device, *m_Image, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // TODO: Separate samplers from textures
        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = device.GetPhysicalDevice().Properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (const auto result = vkCreateSampler(device.Get(), &samplerInfo, nullptr, &m_Sampler); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create vulkan texture sampler! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }

        m_BindlessIndex = backend.GetTextureRegistry().AddTexture(this); // Add to registry
        Debug::Message("Bindless index: {}.", m_BindlessIndex);
    }

    VK_Texture::~VK_Texture()
    {
        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = renderer.GetBackend();

        vkDestroySampler(backend.GetDevice().Get(), m_Sampler, nullptr);

        backend.GetTextureRegistry().RemoveTexture(m_BindlessIndex); // remove from registry
    }
}

