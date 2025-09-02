#include "VK_BindlessManager.hpp"
#include "Wrapper/VK_Image.hpp"
#include "Wrapper/VK_MemoryBuffer.hpp"
#include "AssetBackends/VK_Texture.hpp"
#include "Wrapper/VK_DescriptorPool.hpp"
#include "Helpers/VulkanUtility.hpp"
#include "../ShaderStructs.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/AssetManager/BuiltInAssets.hpp"
#include "Subsystems/SubsystemGetters.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_BindlessManager::VK_BindlessManager(VK_Renderer& renderer, VK_Device& device)
        : m_Renderer(renderer), m_Device(device)
    {
        Debug::Trace("Creating vulkan bindless resources manager.");

        std::vector<VkDescriptorPoolSize> poolSizes(2);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = MAX_TEXTURES;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[1].descriptorCount = 1;

        CreateDescriptorSetLayout();

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, 1, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);
        m_DescriptorSet = m_DescriptorPool->Allocate(m_DescriptorSetLayout);

        CreateMaterialsBuffer();
    }

    VK_BindlessManager::~VK_BindlessManager()
    {
        Debug::Trace("Destroying vulkan bindless resources manager.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        for (const auto& sampler : m_Samplers | std::views::values)
            vkDestroySampler(m_Device.Get(), sampler, nullptr);
    }

    void VK_BindlessManager::CreateMaterialsBuffer()
    {
        m_MaterialsBuffer = std::make_unique<VK_MemoryBuffer>(m_Device, sizeof(MaterialData) * MAX_MATERIALS,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = m_MaterialsBuffer->Get();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        auto write = MakeInfo<VkWriteDescriptorSet>();
        write.dstSet = m_DescriptorSet;
        write.dstBinding = MATERIAL_ARRAY_BINDING;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Device.Get(), 1, &write, 0, nullptr);
    }

    void VK_BindlessManager::CreateDescriptorSetLayout()
    {
        std::array<VkDescriptorSetLayoutBinding, SET_BINDINGS_COUNT> bindings{};
        std::array<VkDescriptorBindingFlags, SET_BINDINGS_COUNT> flags{};
        std::array<VkDescriptorType, SET_BINDINGS_COUNT> types{};

        // Textures array
        bindings[TEXTURE_ARRAY_BINDING].binding = TEXTURE_ARRAY_BINDING;
        bindings[TEXTURE_ARRAY_BINDING].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[TEXTURE_ARRAY_BINDING].descriptorCount = MAX_TEXTURES;
        bindings[TEXTURE_ARRAY_BINDING].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[TEXTURE_ARRAY_BINDING].pImmutableSamplers = nullptr;

        // Materials array
        bindings[MATERIAL_ARRAY_BINDING].binding = MATERIAL_ARRAY_BINDING;
        bindings[MATERIAL_ARRAY_BINDING].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[MATERIAL_ARRAY_BINDING].descriptorCount = 1;
        bindings[MATERIAL_ARRAY_BINDING].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[MATERIAL_ARRAY_BINDING].pImmutableSamplers = nullptr;

        flags[TEXTURE_ARRAY_BINDING] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        flags[MATERIAL_ARRAY_BINDING] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        types[TEXTURE_ARRAY_BINDING] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        types[MATERIAL_ARRAY_BINDING] = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        auto bindingFlags = MakeInfo<VkDescriptorSetLayoutBindingFlagsCreateInfo>();
        bindingFlags.bindingCount = flags.size();
        bindingFlags.pBindingFlags = flags.data();
        bindingFlags.pNext = nullptr;

        auto createInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        createInfo.bindingCount = bindings.size();
        createInfo.pBindings = bindings.data();
        createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        createInfo.pNext = &bindingFlags;

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &createInfo, nullptr, &m_DescriptorSetLayout), "Failed to create bindless descriptor set layout");
    }

    VkSampler VK_BindlessManager::GetSamplerByProperties(const Texture2D::SamplerProperties& properties)
    {
        // check if a sampler with that properties already exists
        {
            std::unique_lock lock(m_SamplersMutex);

            for (const auto& [curProperties, curSampler] : m_Samplers)
            {
                if (curProperties == properties)
                    return curSampler;
            }
        }

        auto samplerInfo = MakeInfo<VkSamplerCreateInfo>();
        samplerInfo.magFilter = static_cast<VkFilter>(properties.MagFilter);
        samplerInfo.minFilter = static_cast<VkFilter>(properties.MinFilter);
        samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(properties.AddressModeU);
        samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(properties.AddressModeV);
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_Device.GetPhysicalDevice().Properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = static_cast<VkBorderColor>(properties.BorderColor);
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = properties.EnableMips ? VK_LOD_CLAMP_NONE : 0.0f;

        VkSampler sampler = VK_NULL_HANDLE;
        VK_CHECK_RESULT(vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &sampler), "Failed to create texture sampler!");

        {
            std::unique_lock lock(m_SamplersMutex);
            m_Samplers.emplace_back(properties, sampler);
        }

        return sampler;
    }

    void VK_BindlessManager::UpdateTextureDescriptor(VkImageView imageView, VkSampler sampler, const uint32_t slot) const
    {
        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        auto write = MakeInfo<VkWriteDescriptorSet>();
        write.dstSet = m_DescriptorSet;
        write.dstBinding = TEXTURE_ARRAY_BINDING;
        write.dstArrayElement = slot;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device.Get(), 1, &write, 0, nullptr);
    }

    uint32_t VK_BindlessManager::AddTexture(const Ref<VK_Texture> texture)
    {
        uint32_t slotIndex = UINT32_MAX;

        {
            std::unique_lock lock(m_TexturesMutex);

            if (!m_FreeTextureSlots.empty())
            {
                slotIndex = m_FreeTextureSlots.front();
                m_FreeTextureSlots.pop();
                m_Textures[slotIndex] = texture;
            }
            else
            {
                slotIndex = m_Textures.size();
                ASSERT(slotIndex < MAX_TEXTURES, "Exceeded the maximum number of bindless textures!");
                m_Textures.emplace_back(texture);
            }
        }

        UpdateTextureDescriptor(
            texture->GetImage().GetView(),
            GetSamplerByProperties(texture->GetSamplerProperties()),
            slotIndex
        );

        return slotIndex;
    }

    void VK_BindlessManager::RemoveTexture(const uint32_t textureIndex)
    {
        // Default textures should never be removed because doing so will cause an infinite
        // loop of unloading and loading BuiltInAssets::TextureError
        if (textureIndex == 0 || textureIndex == 1 || textureIndex == 2)
            return;

        static Ref<VK_Texture> defaultTexture = nullptr;
        if (!defaultTexture)
        {
            const auto hTex = GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureError);
            defaultTexture = hTex->GetImpl();
        }

        {
            std::unique_lock lock(m_TexturesMutex);

            if (textureIndex >= m_Textures.size() || !m_Textures.at(textureIndex))
            {
                Debug::Error("{} is not a valid bindless texture index!", textureIndex);
                return;
            }

            m_Textures[textureIndex] = nullptr;
            m_FreeTextureSlots.push(textureIndex);
        }

        UpdateTextureDescriptor(
            defaultTexture->GetImage().GetView(),
            GetSamplerByProperties(defaultTexture->GetSamplerProperties()),
            textureIndex
        );
    }

    void VK_BindlessManager::SetTextureSampler(const Ref<VK_Texture> texture, const Texture2D::SamplerProperties& samplerProperties)
    {
        const auto textureIndex = texture->GetBindlessIndex();

        {
            std::unique_lock lock(m_TexturesMutex);

            if (textureIndex >= m_Textures.size() || !m_Textures.at(textureIndex))
            {
                Debug::Error("{} is not a valid bindless texture index!", textureIndex);
                return;
            }
        }

        UpdateTextureDescriptor(
            texture->GetImage().GetView(),
            GetSamplerByProperties(samplerProperties),
            textureIndex
        );
    }

    uint32_t VK_BindlessManager::AddMaterial(const Ref<MaterialData> material)
    {
        uint32_t slotIndex = UINT32_MAX;

        {
            std::unique_lock lock(m_MaterialsMutex);

            if (!m_FreeMaterialSlots.empty())
            {
                slotIndex = m_FreeTextureSlots.front();
                m_FreeMaterialSlots.pop();
                m_Materials[slotIndex] = *material;
            }
            else
            {
                slotIndex = m_Materials.size();
                ASSERT(slotIndex < MAX_MATERIALS, "Exceeded the maximum number of bindless materials!");
                m_Materials.emplace_back(*material);
            }
        }

        m_MaterialsBuffer->UploadData(0, sizeof(MaterialData) * m_Materials.size(), m_Materials.data());

        return slotIndex;
    }

    void VK_BindlessManager::RemoveMaterial(const uint32_t materialIndex)
    {
        {
            std::unique_lock lock(m_MaterialsMutex);

            if (materialIndex >= m_Materials.size())
            {
                Debug::Error("{} is not a valid bindless material index!", materialIndex);
                return;
            }

            m_Materials[materialIndex] = MaterialData();
            m_FreeMaterialSlots.push(materialIndex);
        }

        m_MaterialsBuffer->UploadData(0, sizeof(MaterialData) * m_Materials.size(), m_Materials.data());
    }
}
