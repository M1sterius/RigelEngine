#pragma once

#include "Core.hpp"
#include "QueueType.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_CmdPool
    {
    public:
        VK_CmdPool(VK_Device& device, const QueueType queueType);
        ~VK_CmdPool();

        VK_CmdPool(const VK_CmdPool&) = delete;
        VK_CmdPool operator = (const VK_CmdPool&) = delete;

        NODISCARD VkCommandPool Get() const { return m_CommandPool; }

        NODISCARD VkCommandBuffer Allocate() const;
        void Free(VkCommandBuffer commandBuffer) const;
    private:
        VK_Device& m_Device;

        QueueType m_QueueType;
        uint32_t m_QueueFamilyIndex = -1;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    };
}
