#include "TextureRegistry.hpp"

#include "Time.hpp"
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
        m_DescriptorSet = CreateDescriptorSet();
    }

    TextureRegistry::~TextureRegistry()
    {
        Debug::Trace("Destroying vulkan bindless textures registry.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        vkFreeDescriptorSets(m_Device.Get(), m_DescriptorPool->Get(), 1, &m_DescriptorSet);

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

    VkDescriptorSet TextureRegistry::CreateDescriptorSet() const
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

        if (const auto result = vkAllocateDescriptorSets(m_Device.Get(), &allocInfo, &descriptorSet); result != VK_SUCCESS)
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                std::format("Failed to create descriptor set for vulkan texture registry! VkResult: {}.", static_cast<int32_t>(result)), __FILE__, __LINE__);
        }

        return descriptorSet;
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

        // put the new texture at the selected slot via vkUpdateDescriptorSets
        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageView = texture->GetImage().GetView();
        imageInfo.sampler = texture->GetSampler();
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

        return slotIndex;
    }

    void TextureRegistry::RemoveTexture(const uint32_t textureIndex)
    {

    }

    VkDescriptorSet TextureRegistry::GetDescriptorSet() const
    {
        return m_DescriptorSet;
    }
}
