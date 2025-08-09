#pragma once

#include "Core.hpp"
#include "../../../../Include/Assets/Texture.hpp"
#include "AssetHandle.hpp"
#include "ShaderStructs.hpp"

#include "vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_Texture;
    class VK_DescriptorPool;
    class VK_MemoryBuffer;

    struct SceneData;

    class VK_BindlessManager 
    {
    public:
        static constexpr uint32_t MAX_TEXTURES = 4096;
        static constexpr uint32_t MAX_MATERIALS = SceneData::MAX_MATERIAL_DATA_ARRAY_SIZE;

        enum SetBindings : uint32_t
        {
            TEXTURE_ARRAY_BINDING = 0,
            STORAGE_BUFFER_ARRAY_BINDING = 1
        };

        VK_BindlessManager(VK_Renderer& renderer, VK_Device& device);
        ~VK_BindlessManager();

        NODISCARD inline VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
        NODISCARD inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

        void UpdateStorageBuffer();

        NODISCARD uint32_t AddTexture(const Ref<VK_Texture> texture);
        void RemoveTexture(const uint32_t textureIndex);
        void SetTextureSampler(const Ref<VK_Texture> texture, const Texture::SamplerProperties& samplerProperties);

        NODISCARD uint32_t AddMaterial(const Ref<MaterialData> material);
        void RemoveMaterial(const uint32_t materialIndex);
    private:
        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        void CreateStorageBuffers();
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

        std::vector<Ref<MaterialData>> m_Materials;
        std::mutex m_MaterialMutex;

        std::vector<std::unique_ptr<VK_MemoryBuffer>> m_StorageBuffers;
        std::vector<bool> m_DirtyBufferFlags;
        std::unique_ptr<SceneData> m_SceneData;
    };
}
