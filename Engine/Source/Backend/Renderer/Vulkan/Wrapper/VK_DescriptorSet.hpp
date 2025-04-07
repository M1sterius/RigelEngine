#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <vector>

namespace Rigel::Backend
{
    class VK_Device;
    class VK_UniformBuffer;
    class VK_DescriptorPool;

    class VK_DescriptorSetBuilder
    {
    public:
        explicit VK_DescriptorSetBuilder(VK_Device& device);
        ~VK_DescriptorSetBuilder();

        VK_DescriptorSetBuilder(const VK_DescriptorSetBuilder&) = delete;
        VK_DescriptorSetBuilder operator = (const VK_DescriptorSetBuilder) = delete;

        NODISCARD VkDescriptorSetLayout BuildLayout() const;

        void AddUniformBuffer(const VK_UniformBuffer& uniformBuffer, const uint32_t binding,
            VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_VERTEX_BIT);

        // TODO: Implement AddCombinedImageSampler()
    private:
        VK_Device& m_Device;

        std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings;
        std::vector<VkWriteDescriptorSet> m_DescriptorWrites;

        std::vector<VkDescriptorBufferInfo> m_UniformBufferInfos;
        std::vector<VkDescriptorImageInfo> m_DescriptorImageInfos;

        friend class VK_DescriptorSet;
    };

    class VK_DescriptorSet 
    {
    public:
        VK_DescriptorSet(VK_Device& device, VK_DescriptorPool& pool, const VK_DescriptorSetBuilder& descriptorBuild);
        ~VK_DescriptorSet();

        VK_DescriptorSet(const VK_DescriptorSet&) = delete;
        VK_DescriptorSet operator = (const VK_DescriptorSet) = delete;

        NODISCARD VkDescriptorSet Get() const { return m_DescriptorSet; }
    private:
        VK_Device& m_Device;
        VK_DescriptorPool& m_Pool;

        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    };
}
