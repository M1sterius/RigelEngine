#include "VK_BindlessManager.hpp"
#include "AssetManager.hpp"
#include "BuiltInAssets.hpp"
#include "VulkanUtility.hpp"
#include "VK_DescriptorPool.hpp"
#include "VK_Image.hpp"
#include "VK_Texture.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_BindlessManager::VK_BindlessManager(VK_Renderer& renderer, VK_Device& device)
        : m_Renderer(renderer), m_Device(device)
    {
        Debug::Trace("Creating vulkan bindless resources manager.");

        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = MAX_TEXTURES;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, 1, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);

        CreateDescriptorSetLayout();
        CreateDescriptorSet();
    }

    VK_BindlessManager::~VK_BindlessManager()
    {
        Debug::Trace("Destroying vulkan bindless resources manager.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        for (const auto& sampler : m_TextureSamplers | std::views::values)
            vkDestroySampler(m_Device.Get(), sampler, nullptr);
    }

    void VK_BindlessManager::CreateDescriptorSetLayout()
    {
        constexpr uint32_t bindingCount = 1;

        std::array<VkDescriptorSetLayoutBinding, bindingCount> bindings;
        std::array<VkDescriptorBindingFlags, bindingCount> flags;
        std::array<VkDescriptorType, bindingCount> types;

        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].descriptorCount = MAX_TEXTURES;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[0].pImmutableSamplers = nullptr;

        flags[0] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        types[0] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
        bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingFlags.pBindingFlags = flags.data();
        bindingFlags.bindingCount = bindingCount;
        bindingFlags.pNext = nullptr;

        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = bindingCount;
        createInfo.pBindings = bindings.data();
        createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        createInfo.pNext = &bindingFlags;

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &createInfo, nullptr, &m_DescriptorSetLayout), "Failed to create bindless descriptor set layout");
    }

    void VK_BindlessManager::CreateDescriptorSet()
    {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = m_DescriptorPool->Get();
        allocateInfo.pSetLayouts = &m_DescriptorSetLayout;
        allocateInfo.descriptorSetCount = 1;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.Get(), &allocateInfo, &m_DescriptorSet), "Failed to create bindless descriptor set!");
    }

    VkSampler VK_BindlessManager::GetSamplerByProperties(const Texture::SamplerProperties& properties)
    {
        // check if a sampler with that properties already exists
        {
            std::unique_lock lock(m_SamplersMutex);

            for (const auto& [curProperties, curSampler] : m_TextureSamplers)
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

        VkSampler sampler = VK_NULL_HANDLE;
        VK_CHECK_RESULT(vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &sampler), "Failed to create texture sampler!");

        {
            std::unique_lock lock(m_SamplersMutex);
            m_TextureSamplers.emplace_back(properties, sampler);
        }

        return sampler;
    }

    void VK_BindlessManager::UpdateTextureDescriptor(VkImageView imageView, VkSampler sampler, const uint32_t slotIndex) const
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

    uint32_t VK_BindlessManager::AddTexture(const Ref<VK_Texture> texture, const Texture::SamplerProperties& samplerProperties)
    {
        uint32_t slotIndex = UINT32_MAX;

        // checks if there is an empty slot not in the end of the vector
        {
            std::unique_lock lock(m_TextureMutex);

            for (uint32_t i = 0; i < m_Textures.size(); ++i)
            {
                if (m_Textures.at(i).IsNull())
                {
                    slotIndex = i;
                    break;
                }
            }
        }

        // haven't found an empty slot, so we push a new one to the end of the vector
        if (slotIndex == UINT32_MAX)
        {
            slotIndex = m_Textures.size();

            if (slotIndex >= MAX_TEXTURES)
            {
                Debug::Crash(ErrorCode::LIMIT_EXCEEDED,
                    "Exceeded the maximum number of bindless textures!", __FILE__, __LINE__);
            }

            {
                std::unique_lock lock(m_TextureMutex);
                m_Textures.push_back(nullptr);
            }
        }

        {
            std::unique_lock lock (m_TextureMutex);
            m_Textures[slotIndex] = texture;
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
        // if the engine is shutting down we no longer care about maintaining proper free list
        // also it fixes an infinite loop when removing the default texture during shutdown
        if (!Engine::Get().Running())
            return;

        static Ref<VK_Texture> defaultTexture = nullptr;
        if (defaultTexture.IsNull())
        {
            auto& manager = Engine::Get().GetAssetManager();
            const auto hTex = manager.Load<Texture>(BuiltInAssets::TextureError);
            defaultTexture = hTex->GetImpl();
        }

        {
            std::unique_lock lock(m_TextureMutex);

            if (textureIndex >= m_Textures.size() || m_Textures.at(textureIndex).IsNull())
            {
                Debug::Error("{} is not a valid bindless texture index!", textureIndex);
                return;
            }
        }

        {
            std::unique_lock lock(m_TextureMutex);
            m_Textures[textureIndex] = nullptr;
        }

        UpdateTextureDescriptor(
            defaultTexture->GetImage().GetView(),
            GetSamplerByProperties(defaultTexture->GetSamplerProperties()),
            textureIndex
        );
    }

    void VK_BindlessManager::SetTextureSampler(const Ref<VK_Texture> texture, const Texture::SamplerProperties& samplerProperties)
    {
        const auto textureIndex = texture->GetBindlessIndex();

        {
            std::unique_lock lock(m_TextureMutex);

            if (textureIndex >= m_Textures.size() || m_Textures.at(textureIndex).IsNull())
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
}
