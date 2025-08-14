#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_DescriptorPool
    {
    public:
        VK_DescriptorPool(VK_Device& device, const std::vector<VkDescriptorPoolSize>& poolSizes, const size_t maxSets, const VkDescriptorPoolCreateFlags flags);
        ~VK_DescriptorPool();

        NODISCARD VkDescriptorPool Get() const { return m_DescriptorPool; }
    private:
        VK_Device& m_Device;

        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    };
}