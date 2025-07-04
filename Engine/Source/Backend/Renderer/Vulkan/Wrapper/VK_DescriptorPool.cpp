#include "VK_DescriptorPool.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_DescriptorPool::VK_DescriptorPool(VK_Device& device, const std::vector<VkDescriptorPoolSize>& poolSizes,
        const size_t maxSets) : m_Device(device)
    {
        ASSERT(!poolSizes.empty(), "Attempted to create an empty descriptor pool");
        ASSERT(maxSets > 0, "Attempted to create an empty descriptor pool");

        auto createInfo = MakeInfo<VkDescriptorPoolCreateInfo>();
        createInfo.poolSizeCount = poolSizes.size();
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.maxSets = maxSets;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // This allows individual sets to be freed

        VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device.Get(), &createInfo, nullptr, &m_DescriptorPool), "Failed to create descriptor pool!");
    }

    VK_DescriptorPool::~VK_DescriptorPool()
    {
        vkDestroyDescriptorPool(m_Device.Get(), m_DescriptorPool, nullptr);
    }
}
