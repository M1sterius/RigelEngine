#pragma once

#include "Core.hpp"
#include "ShaderStructs.hpp"
#include "Assets/Texture2D.hpp"
#include "Handles/AssetHandle.hpp"

#include "vulkan/vulkan.h"

#include <vector>
#include <queue>

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
    private:
        static constexpr uint32_t SET_BINDINGS_COUNT = 2;
        static constexpr uint32_t MAX_TEXTURES = 2048;
        static constexpr uint32_t MAX_MATERIALS = 512;

        enum SetBindings : uint32_t
        {
            TEXTURE_ARRAY_BINDING = 0,
            MATERIAL_ARRAY_BINDING = 1,
        };
    public:
        static constexpr uint32_t ERROR_TEXTURE_BINDLESS_INDEX = 0;
        static constexpr uint32_t BLACK_TEXTURE_BINDLESS_INDEX = 1;
        static constexpr uint32_t WHITE_TEXTURE_BINDLESS_INDEX = 2;

        VK_BindlessManager(VK_Renderer& renderer, VK_Device& device);
        ~VK_BindlessManager();

        VK_BindlessManager(const VK_BindlessManager&) = delete;
        VK_BindlessManager operator = (const VK_BindlessManager&) = delete;

        NODISCARD inline VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
        NODISCARD inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

        NODISCARD uint32_t AddTexture(const Ref<VK_Texture> texture);
        void RemoveTexture(const uint32_t textureIndex);
        void SetTextureSampler(const Ref<VK_Texture> texture, const Texture2D::SamplerProperties& samplerProperties);

        NODISCARD uint32_t AddMaterial(const Ref<MaterialData> material);
        void RemoveMaterial(const uint32_t materialIndex);
    private:
        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        void CreateMaterialsBuffer();
        void CreateDescriptorSetLayout();

        NODISCARD VkSampler GetSamplerByProperties(const Texture2D::SamplerProperties& properties);
        void UpdateTextureDescriptor(VkImageView imageView, VkSampler sampler, const uint32_t slot) const;

        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        std::vector<std::pair<Texture2D::SamplerProperties, VkSampler>> m_Samplers;
        std::mutex m_SamplersMutex;

        std::vector<Ref<VK_Texture>> m_Textures;
        std::queue<uint32_t> m_FreeTextureSlots;
        std::mutex m_TexturesMutex;

        std::vector<MaterialData> m_Materials;
        std::queue<uint32_t> m_FreeMaterialSlots;
        std::mutex m_MaterialsMutex;

        std::unique_ptr<VK_MemoryBuffer> m_MaterialsBuffer;
    };
}
