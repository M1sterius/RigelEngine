#include "VK_Texture.hpp"
#include "../Wrapper/VK_Image.hpp"
#include "../Wrapper/VK_MemoryBuffer.hpp"
#include "../Wrapper/VK_CmdBuffer.hpp"
#include "../Helpers/VulkanUtility.hpp"
#include "../VK_BindlessManager.hpp"
#include "../VK_StagingManager.hpp"

inline VkFormat DeduceFormat(const uint32_t components, const bool linear)
{
    const auto cmp = std::clamp(components, 1u, 4u);

    if (linear)
    {
        switch (cmp)
        {
        case 1: return VK_FORMAT_R8_UNORM;
        case 2: return VK_FORMAT_R8G8_UNORM;
        case 3: return VK_FORMAT_R8G8B8A8_UNORM; // Because most GPUs don't support 24-bit textures
        default: return VK_FORMAT_R8G8B8A8_UNORM;
        }
    }
    else
    {
        switch (cmp)
        {
        case 1: return VK_FORMAT_R8_SRGB;
        case 2: return VK_FORMAT_R8G8_SRGB;
        case 3: return VK_FORMAT_R8G8B8A8_SRGB; // Because most GPUs don't support 24-bit textures
        default: return VK_FORMAT_R8G8B8A8_SRGB;
        }
    }
}

namespace Rigel::Backend::Vulkan
{
    VK_Texture::VK_Texture(const void* pixelData, const glm::uvec2 size, const uint32_t components,
            const bool linear, const uint32_t mipLevelCount)
    {
        const VkDeviceSize imageSize = size.x * size.y * components; // every component is guaranteed to be 8 bits

        auto& renderer = GetVKRenderer();
        auto& device = renderer.GetDevice();

        auto& stagingBuffer = renderer.GetStagingManager().GetBuffer();
        stagingBuffer.UploadData(0, imageSize, pixelData);

        m_Image = std::make_unique<VK_Image>(device, size, DeduceFormat(components, linear), VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, mipLevelCount);

        m_Image->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_Image::AllMips);
        m_Image->CopyFromBuffer(stagingBuffer);
        GenerateMipMaps();

        m_BindlessIndex = GetVKRenderer().GetBindlessManager().AddTexture(this);
    }

    void VK_Texture::GenerateMipMaps() const
    {
        auto& device = GetDevice();

        int32_t mipWidth = static_cast<int32_t>(GetSize().x);
        int32_t mipHeight = static_cast<int32_t>(GetSize().y);

        const auto cmdBuff = VK_CmdBuffer::BeginSingleTime(device, QueueType::Graphics);
        for (uint32_t i = 0; i < m_Image->GetMipLevelCount() - 1; ++i)
        {
            const auto srcMip = i;
            const auto dstMip = i + 1;

            const auto vkImage = m_Image->Get();

            VK_Image::CmdTransitionLayout(cmdBuff->Get(), vkImage,
                m_Image->GetAspectFlags(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcMip);

            VkImageBlit blit {};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = srcMip;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = dstMip;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(cmdBuff->Get(),
                vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;

            VK_Image::CmdTransitionLayout(cmdBuff->Get(), vkImage,
                m_Image->GetAspectFlags(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcMip);
        }
        VK_CmdBuffer::EndSingleTime(cmdBuff);
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
