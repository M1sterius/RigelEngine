#include "VK_Fence.hpp"
#include "../Helpers/VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_Fence::VK_Fence(const VK_Device& device, const bool signalBit)
            :   m_Device(device)
    {
        auto fenceInfo = MakeInfo<VkFenceCreateInfo>();
        if (signalBit) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_CHECK_RESULT(vkCreateFence(m_Device.Get(), &fenceInfo, nullptr, &m_Fence), "Failed to create fence!");
    }

    VK_Fence::~VK_Fence()
    {
        vkDestroyFence(m_Device.Get(), m_Fence, nullptr);
    }

    void VK_Fence::Reset() const
    {
        VK_CHECK_RESULT(vkResetFences(m_Device.Get(), 1, &m_Fence), "Failed to reset fence!");
    }

    void VK_Fence::Wait(const uint64_t timeout) const
    {
        VK_CHECK_RESULT(vkWaitForFences(m_Device.Get(), 1, &m_Fence, VK_TRUE, timeout), "Failed to wait for fence!");
    }

    void VK_Fence::WaitAndReset(const uint64_t timeout)
    {
        Wait(timeout);
        Reset();
    }
}
