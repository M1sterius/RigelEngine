#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <utility>

namespace rge::backend
{
    struct PhysicalDeviceInfo
    {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties Properties = { };
    };

    class VK_Device
    {
    public:
        explicit VK_Device(VkInstance instance);
        ~VK_Device();

        VK_Device(const VK_Device&) = delete;
        VK_Device operator = (const VK_Device) = delete;
    private:
        VkInstance m_Instance;
        VkPhysicalDevice m_SelectedPhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        NODISCARD std::vector<PhysicalDeviceInfo> FindPhysicalDevices();
        NODISCARD VkPhysicalDevice PickBestPhysicalDevice(const std::vector<VkPhysicalDevice>& availableDevices);
    };
}
