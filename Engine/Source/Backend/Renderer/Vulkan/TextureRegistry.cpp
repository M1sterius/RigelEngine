#include "TextureRegistry.hpp"
#include "AssetManager.hpp"
#include "BuiltInAssets.hpp"
#include "Engine.hpp"
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
        m_Registry = std::vector<VK_Texture*>(MAX_TEXTURES);

        CreateDescriptorSetLayout();
        CreateDescriptorSet();
        CreateDefaultSampler();
    }

    TextureRegistry::~TextureRegistry()
    {
        Debug::Trace("Destroying vulkan bindless textures registry.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        vkFreeDescriptorSets(m_Device.Get(), m_DescriptorPool->Get(), 1, &m_DescriptorSet);
        vkDestroySampler(m_Device.Get(), m_DefaultSampler, nullptr);

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
        constexpr VkDescriptorBindingFlagsEXT bindingFlags =
            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
            VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsInfo {};
        bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        bindingFlagsInfo.bindingCount = 1;
        bindingFlagsInfo.pBindingFlags = &bindingFlags;

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &binding;
        layoutInfo.pNext = &bindingFlagsInfo;

        if (const auto result = vkCreateDescriptorSetLayout(m_Device.Get(), &layoutInfo, nullptr, &m_DescriptorSetLayout); result != VK_SUCCESS)
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

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        if (const auto result = vkAllocateDescriptorSets(m_Device.Get(), &allocInfo, &m_DescriptorSet); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create descriptor set for vulkan texture registry! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }
    }

    void TextureRegistry::CreateDefaultSampler()
    {
        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_Device.GetPhysicalDevice().Properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (const auto result = vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &m_DefaultSampler); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create vulkan texture sampler! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }
    }

    void TextureRegistry::UpdateDescriptorSet(const VK_Image& image, const uint32_t slotIndex) const
    {
        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageView = image.GetView();
        imageInfo.sampler = m_DefaultSampler;
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

    uint32_t TextureRegistry::AddTexture(VK_Texture* texture)
    {
        uint32_t slotIndex = -1;

        {
            std::unique_lock lock(m_RegistryMutex);

            for (uint32_t i = 0; i < m_Registry.size(); ++i)
            {
                if (m_Registry.at(i) == nullptr)
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

        UpdateDescriptorSet(texture->GetImage(), slotIndex);

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

            if (const auto texture = m_Registry.at(textureIndex); !texture)
            {
                Debug::Error("Failed to remove a bindless texture from the registry! Texture index {} is not occupied in the registry.");
                return;
            }

            m_Registry[textureIndex] = nullptr;
        }

        if (m_DefaultTexture.IsNull())
        {
            auto& manager = Engine::Get().GetAssetManager();
            m_DefaultTexture = manager.Load<Texture>(BuiltInAssets::TextureError);
        }

        const auto& vkTexture = m_DefaultTexture->GetBackend();
        UpdateDescriptorSet(vkTexture.GetImage(), textureIndex);
    }

    VkDescriptorSet TextureRegistry::GetDescriptorSet() const
    {
        return m_DescriptorSet;
    }
}
