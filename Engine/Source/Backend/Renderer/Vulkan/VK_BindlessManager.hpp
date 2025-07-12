#pragma once

#include "Core.hpp"
#include "Texture.hpp"
#include "AssetHandle.hpp"

#include "vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_Texture;
    class VK_DescriptorPool;
    class VK_Image;

    class VK_BindlessManager 
    {
    public:
        static constexpr uint32_t MAX_TEXTURES = 4096;

        VK_BindlessManager(VK_Renderer& renderer, VK_Device& device);
        ~VK_BindlessManager();

        NODISCARD uint32_t AddTexture(const Ref<VK_Texture> texture, const Texture::SamplerProperties& samplerProperties);
        void RemoveTexture(const uint32_t textureIndex);
        void SetTextureSampler(const Ref<VK_Texture> texture, const Texture::SamplerProperties& samplerProperties);
    private:
        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        void CreateDescriptorSetLayout();
        void CreateDescriptorSet();

        NODISCARD VkSampler GetSamplerByProperties(const Texture::SamplerProperties& properties);
        void UpdateTextureDescriptor(VkImageView imageView, VkSampler sampler, const uint32_t slotIndex) const;

        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        std::vector<std::pair<Texture::SamplerProperties, VkSampler>> m_TextureSamplers;
        std::vector<Ref<VK_Texture>> m_Textures;
        std::mutex m_TextureMutex;
        std::mutex m_SamplersMutex;
    };
}
