#if 0
#include "VK_DescriptorSet.hpp"
#include "VK_DescriptorPool.hpp"
#include "VK_MemoryBuffer.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{

    VK_DescriptorSetBuilder::VK_DescriptorSetBuilder(VK_Device& device)
        : m_Device(device) { }
    VK_DescriptorSetBuilder::~VK_DescriptorSetBuilder() = default;

    VkDescriptorSetLayout VK_DescriptorSetBuilder::BuildLayout() const
    {
        auto layoutInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        layoutInfo.bindingCount = m_LayoutBindings.size();
        layoutInfo.pBindings = m_LayoutBindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &layoutInfo, nullptr, &layout), "Failed to create descriptor set layout!");

        return layout;
    }

    void VK_DescriptorSetBuilder::AddUniformBuffer(const VK_UniformBuffer& uniformBuffer, const uint32_t binding,
                                                   const VkShaderStageFlagBits shaderStage)
    {
        m_UniformBufferInfos.push_back({
            .buffer = uniformBuffer.GetMemoryBuffer().Get(),
            .offset = 0,
            .range = uniformBuffer.GetMemoryBuffer().GetSize()
        });

        m_LayoutBindings.push_back({
            .binding = binding,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = static_cast<VkShaderStageFlags>(shaderStage)
        });

        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = nullptr; // Will be filled later
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &m_UniformBufferInfos.back();

        m_DescriptorWrites.push_back(write);
    }

    VK_DescriptorSet::VK_DescriptorSet(VK_Device& device, VK_DescriptorPool& pool,
        const VK_DescriptorSetBuilder& descriptorBuild)
        : m_Device(device), m_Pool(pool)
    {
        m_DescriptorSetLayout = descriptorBuild.BuildLayout();

        auto allocInfo = MakeInfo<VkDescriptorSetAllocateInfo>();
        allocInfo.descriptorPool = pool.Get();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_DescriptorSetLayout;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.Get(), &allocInfo, &m_DescriptorSet), "Failed to allocate descriptor set!");

        auto descriptorWrites = descriptorBuild.m_DescriptorWrites;
        for (auto& write : descriptorWrites)
            write.dstSet = m_DescriptorSet;

        vkUpdateDescriptorSets(m_Device.Get(), descriptorWrites.size(),
            descriptorWrites.data(), 0, nullptr);
    }

    VK_DescriptorSet::~VK_DescriptorSet()
    {
        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
        vkFreeDescriptorSets(m_Device.Get(), m_Pool.Get(), 1, &m_DescriptorSet);
    }
}
#endif
