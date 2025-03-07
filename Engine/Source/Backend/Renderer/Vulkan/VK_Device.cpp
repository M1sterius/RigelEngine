#include "VK_Device.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"

#include <format>
#include <set>

namespace rge::backend
{
    VK_Device::VK_Device(VkInstance instance, VkSurfaceKHR surface)
        : m_Instance(instance), m_Surface(surface)
    {
        RGE_TRACE("Searching for GPUs with Vulkan support.");
        auto availableDevices = FindPhysicalDevices(m_Instance);
        RGE_TRACE("Detected GPUs:");
        for (const auto& device : availableDevices)
            RGE_TRACE(std::format("    {}", device.Properties.deviceName));

        m_SelectedPhysicalDevice = PickBestPhysicalDevice(availableDevices, m_Surface);
        if (m_SelectedPhysicalDevice.IsNull())
            Debug::ThrowError("Failed to find any GPUs with adequate support of required features!");
    }

    VK_Device::~VK_Device()
    {

    }

    std::vector<PhysicalDeviceInfo> VK_Device::FindPhysicalDevices(VkInstance instance)
    {
        auto devices = std::vector<VkPhysicalDevice>();

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            Debug::ThrowError("Failed to find GPUs with Vulkan support!");

        devices.resize(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

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

    PhysicalDeviceInfo VK_Device::PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface)
    {
        return availableDevices[0]; // TEMP!!!
    }

#pragma region PhysicalDeviceChecks

    bool VK_Device::CheckPhysicalDeviceExtensionsSupport(VkPhysicalDevice device)
    {
        // Checks if all extensions specified in VK_Config::RequiredPhysicalDeviceExtensions
        // are supported by a device

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        auto availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(VK_Config::RequiredPhysicalDeviceExtensions.begin(), VK_Config::RequiredPhysicalDeviceExtensions.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VK_Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // Searches for support of graphics and present queue families on device,
        // corresponding std::optional in QueueFamilyIndices won't have a values if the queue isn't supported

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.GraphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
                indices.PresentFamily = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    SwapChainSupportDetails VK_Device::QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // Retrieve information about swap chain support details on a device

        SwapChainSupportDetails details;

        // Supported surface capabilities (e.g. min/max number of swapchain images, min/max swapchain image extents, etc.)
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

        // Supported swapchain surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
        }

        // Supported swapchain surface present modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }

#pragma endregion
}