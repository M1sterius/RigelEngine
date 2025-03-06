#include "VK_Device.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"

#include <format>
#include <set>

namespace rge::backend
{
    VK_Device::VK_Device(VkInstance instance)
        : m_Instance(instance)
    {
        RGE_TRACE("Searching for GPUs with Vulkan support.");
        auto availableDevices = FindPhysicalDevices();
        RGE_TRACE("Detected GPUs:");
        for (const auto& device : availableDevices)
            RGE_TRACE(std::format("    {}", device.Properties.deviceName));
    }

    VK_Device::~VK_Device()
    {

    }

    std::vector<PhysicalDeviceInfo> VK_Device::FindPhysicalDevices()
    {
        auto devices = std::vector<VkPhysicalDevice>();

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            Debug::ThrowError("Failed to find GPUs with Vulkan support!");

        devices.resize(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        auto devicesInfo = std::vector<PhysicalDeviceInfo>(deviceCount);

        // Retrieve the properties of all detected devices
        for (size_t i = 0; i < devices.size(); i++)
        {
            const auto device = devices[i];
            devicesInfo[i].PhysicalDevice = device;

            vkGetPhysicalDeviceProperties(device, &devicesInfo[i].Properties);
        }

        return devicesInfo;
    }

    VkPhysicalDevice VK_Device::PickBestPhysicalDevice(const std::vector<VkPhysicalDevice>& availableDevices)
    {
        return nullptr;
    }

#pragma region PhysicalDeviceChecks

    bool VK_Device::CheckPhysicalDeviceExtensionsSupport(VkPhysicalDevice device)
    {
        // Checks if all required physical device extensions are supported

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        auto availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(VK_Config::RequiredPhysicalDeviceExtensions.begin(), VK_Config::RequiredPhysicalDeviceExtensions.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

#pragma endregion
}