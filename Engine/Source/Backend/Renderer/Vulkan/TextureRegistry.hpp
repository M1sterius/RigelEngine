#pragma once

#include "Core.hpp"
#include "Texture.hpp"
#include "AssetHandle.hpp"
#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_Texture;
    class VK_DescriptorPool;
    class VK_Image;

    class TextureRegistry 
    {
    public:
        static constexpr uint32_t MAX_TEXTURES = 2048;

        explicit TextureRegistry(VK_Renderer& renderer);
        ~TextureRegistry();

        NODISCARD uint32_t AddTexture(VK_Texture* texture);
        void RemoveTexture(uint32_t textureIndex);

        NODISCARD VkDescriptorSet GetDescriptorSet() const;
        NODISCARD VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
    private:
        struct SamplerInfo
        {
            VkSampler Sampler = VK_NULL_HANDLE;
            Texture::SamplerProperties Properties = {};
        };

        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        std::vector<SamplerInfo> m_Samplers;
        std::vector<VK_Texture*> m_Registry;
        mutable std::mutex m_RegistryMutex;

        void UpdateDescriptorSet(const VK_Image& image, const uint32_t slotIndex) const;

        SamplerInfo CreateSampler(const Texture::SamplerProperties& properties) const;
        void CreateDescriptorSetLayout();
        void CreateDescriptorSet();
    };
}
