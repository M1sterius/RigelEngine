#include "VK_DescriptorPool.hpp"
#include "VK_Device.hpp"
#include "VulkanException.hpp"
#include "MakeInfo.hpp"

namespace rge::backend
{
    VK_DescriptorPool::VK_DescriptorPool(VK_Device& device, const std::vector<VkDescriptorPoolSize> poolSizes,
        const size_t maxSets) : m_Device(device)
    {
        ASSERT(!poolSizes.empty(), "Attempted to create an empty descriptor pool");
        ASSERT(maxSets > 0, "Attempted to create an empty descriptor pool");

        auto createInfo = MakeInfo<VkDescriptorPoolCreateInfo>();
        createInfo.poolSizeCount = poolSizes.size();
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.maxSets = maxSets;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // This allows individual sets to be freed

        if (const auto result = vkCreateDescriptorPool(m_Device.Get(), &createInfo, nullptr, &m_DescriptorPool); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan descriptor pool!", result);
    }

    VK_DescriptorPool::~VK_DescriptorPool()
    {
        vkDestroyDescriptorPool(m_Device.Get(), m_DescriptorPool, nullptr);
    }
}
