#include "VK_Fence.hpp"
#include "VK_Device.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

namespace rge::backend
{
    VK_Fence::VK_Fence(VK_Device& device, const bool signalBit)
            :   m_Device(device)
    {
        auto fenceInfo = MakeInfo<VkFenceCreateInfo>();
        if (signalBit) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (const auto result = vkCreateFence(m_Device.Get(), &fenceInfo, nullptr, &m_Fence); result != VK_SUCCESS)
            throw VulkanException("Failed to Vulkan create fence!", result);
    }

    VK_Fence::~VK_Fence()
    {
        vkDestroyFence(m_Device.Get(), m_Fence, nullptr);
    }

    void VK_Fence::Reset()
    {
        if (const auto result = vkResetFences(m_Device.Get(), 1, &m_Fence); result != VK_SUCCESS)
            throw VulkanException("Failed to reset Vulkan fence!", result);
    }

    void VK_Fence::Wait(const uint64_t timeout)
    {
        if (const auto result = vkWaitForFences(m_Device.Get(), 1, &m_Fence, VK_TRUE, timeout); result != VK_SUCCESS)
            throw VulkanException("Failed to wait for Vulkan fence!", result);
    }
}
