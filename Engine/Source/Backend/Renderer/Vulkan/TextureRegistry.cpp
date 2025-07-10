#include "TextureRegistry.hpp"
#include "AssetManager.hpp"
#include "BuiltInAssets.hpp"
#include "Engine.hpp"
#include "MakeInfo.hpp"
#include "Texture.hpp"
#include "VK_Renderer.hpp"
#include "VK_Swapchain.hpp"
#include "VK_DescriptorPool.hpp"
#include "VK_Image.hpp"
#include "VK_Texture.hpp"

namespace Rigel::Backend::Vulkan
{
    TextureRegistry::TextureRegistry(VK_Renderer& renderer)
        : m_Renderer(renderer), m_Device(m_Renderer.GetDevice())
    {
        // TODO: Abstract descriptor set creation into VK_DescriptorSet class

        Debug::Trace("Creating vulkan bindless textures registry of size {}.", MAX_TEXTURES);

        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = MAX_TEXTURES;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, 1);
        m_Registry = std::vector<Ref<VK_Texture>>(MAX_TEXTURES);

        CreateDescriptorSetLayout();
        CreateDescriptorSet();
        m_Samplers.emplace_back(CreateSampler(Texture::SamplerProperties()));
    }

    TextureRegistry::~TextureRegistry()
    {
        Debug::Trace("Destroying vulkan bindless textures registry.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        vkFreeDescriptorSets(m_Device.Get(), m_DescriptorPool->Get(), 1, &m_DescriptorSet);

        for (auto& [sampler, _] : m_Samplers)
            vkDestroySampler(m_Device.Get(), sampler, nullptr);

        m_DescriptorPool.reset();
    }

    void TextureRegistry::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding binding {};
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = MAX_TEXTURES;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        binding.pImmutableSamplers = nullptr;

        // Enable runtime descriptor array & partial binding
        constexpr VkDescriptorBindingFlags bindingFlags =
            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
            VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
            VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo {};
        bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        bindingFlagsInfo.bindingCount = 1;
        bindingFlagsInfo.pBindingFlags = &bindingFlags;

        VkDescriptorSetLayoutCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = 1;
        createInfo.pBindings = &binding;
        createInfo.pNext = &bindingFlagsInfo;
        createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

        if (const auto result = vkCreateDescriptorSetLayout(m_Device.Get(), &createInfo, nullptr, &m_DescriptorSetLayout); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create descriptor set layout for vulkan texture registry! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }
    }

    void TextureRegistry::CreateDescriptorSet()
    {
        VkDescriptorSetVariableDescriptorCountAllocateInfo countInfo {};
        countInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
        countInfo.descriptorSetCount = 1;
        countInfo.pDescriptorCounts = &MAX_TEXTURES;

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool->Get();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_DescriptorSetLayout;
        allocInfo.pNext = &countInfo;

        if (const auto result = vkAllocateDescriptorSets(m_Device.Get(), &allocInfo, &m_DescriptorSet); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create descriptor set for vulkan texture registry! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }
    }

    void TextureRegistry::UpdateDescriptorSet(VkImageView imageView, VkSampler sampler, const uint32_t slotIndex) const
    {
        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSet;
        write.dstBinding = 0;
        write.dstArrayElement = slotIndex;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device.Get(), 1, &write, 0, nullptr);
    }

    TextureRegistry::SamplerInfo TextureRegistry::CreateSampler(const Texture::SamplerProperties& properties) const
    {
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

        VkSampler sampler = VK_NULL_HANDLE;

        if (const auto result = vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &sampler); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create vulkan texture sampler! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }

        return {sampler, properties};
    }

    uint32_t TextureRegistry::AddTexture(const Ref<VK_Texture> texture)
    {
        uint32_t slotIndex = -1;

        {
            std::unique_lock lock(m_RegistryMutex);

            for (uint32_t i = 0; i < m_Registry.size(); ++i)
            {
                if (m_Registry.at(i).IsNull())
                {
                    slotIndex = i;
                    break;
                }
            }

            if (slotIndex >= MAX_TEXTURES)
            {
                Debug::Crash(ErrorCode::VULKAN_BINDLESS_TEXTURE_REGISTRY_OVERFLOW,
                    "Vulkan bindless textures registry is full!", __FILE__, __LINE__);
            }

            m_Registry[slotIndex] = texture;
        }

        UpdateDescriptorSet(texture->GetImage().GetView(), m_Samplers[0].Sampler, slotIndex);

        return slotIndex;
    }

    void TextureRegistry::RemoveTexture(const uint32_t textureIndex)
    {
        ASSERT(textureIndex < m_Registry.size(), "Bindless texture index was out of bound of the registry vector!");

        // if the engine is shutting down then we don't need to maintain the registry anymore
        if (!Engine::Get().Running())
            return;

        {
            std::unique_lock lock(m_RegistryMutex);

            if (const auto texture = m_Registry.at(textureIndex); texture.IsNull())
            {
                Debug::Error("Failed to remove a bindless texture from the registry! Texture index {} is not occupied.");
                return;
            }

            m_Registry[textureIndex] = nullptr;
        }

        static Ref<VK_Texture> defaultTexture = {};
        if (defaultTexture.IsNull())
        {
            auto& manager = Engine::Get().GetAssetManager();
            const auto hTex = manager.Load<Texture>(BuiltInAssets::TextureError);
            defaultTexture = hTex->GetImpl();
        }

        UpdateDescriptorSet(defaultTexture->GetImage().GetView(), m_Samplers[0].Sampler, textureIndex);
    }
}
