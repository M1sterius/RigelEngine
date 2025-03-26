#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <vector>

namespace rge::backend
{
    class VK_Device;

    class VK_DescriptorPool
    {
    public:
        VK_DescriptorPool(VK_Device& device, const std::vector<VkDescriptorPoolSize> poolSizes, const size_t maxSets);
        ~VK_DescriptorPool();

        NODISCARD VkDescriptorPool Get() const { return m_DescriptorPool; }
    private:
        VK_Device& m_Device;

        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    };
}