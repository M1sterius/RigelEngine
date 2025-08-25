#include "VK_DescriptorPool.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_DescriptorPool::VK_DescriptorPool(VK_Device& device, const std::vector<VkDescriptorPoolSize>& poolSizes,
        const size_t maxSets, const VkDescriptorPoolCreateFlags flags) : m_Device(device)
    {
        ASSERT(!poolSizes.empty(), "Attempted to create an empty descriptor pool");
        ASSERT(maxSets > 0, "Attempted to create an empty descriptor pool");

        auto createInfo = MakeInfo<VkDescriptorPoolCreateInfo>();
        createInfo.poolSizeCount = poolSizes.size();
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.maxSets = maxSets;
        createInfo.flags = flags;

        VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device.Get(), &createInfo, nullptr, &m_DescriptorPool), "Failed to create descriptor pool!");
    }

    VK_DescriptorPool::~VK_DescriptorPool()
    {
        vkDestroyDescriptorPool(m_Device.Get(), m_DescriptorPool, nullptr);
    }

    VkDescriptorSet VK_DescriptorPool::Allocate(VkDescriptorSetLayout layout) const
    {
        const VkDescriptorSetLayout layouts[] = {layout};

        auto allocateInfo = MakeInfo<VkDescriptorSetAllocateInfo>();
        allocateInfo.descriptorPool = m_DescriptorPool;
        allocateInfo.pSetLayouts = layouts;
        allocateInfo.descriptorSetCount = 1;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.Get(), &allocateInfo, &descriptorSet), "Failed to allocate descriptor set!");

        return descriptorSet;
    }

    void VK_DescriptorPool::Free(VkDescriptorSet descriptorSet) const
    {
        const VkDescriptorSet sets[] = {descriptorSet};

        VK_CHECK_RESULT(vkFreeDescriptorSets(m_Device.Get(), m_DescriptorPool, 1, sets), "Failed to free descriptor set!");
    }
}
