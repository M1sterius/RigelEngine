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

        RGE_TRACE(std::format("Selected GPU: {}", m_SelectedPhysicalDevice.Properties.deviceName));

        CreateLogicalDevice();
    }

    VK_Device::~VK_Device()
    {

    }

    void VK_Device::CreateLogicalDevice()
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
        // If there is only one GPU we don't have to do anything besides checking it for minimal requirements
        if (availableDevices.size() == 1)
            return IsDeviceSuitable(availableDevices[0].PhysicalDevice, surface) ? availableDevices[0] : PhysicalDeviceInfo();

        PhysicalDeviceInfo chosenDevice {};
        int32_t maxScore = -1000;

        // Extensions required for RT pipeline
        static const std::vector<const char*> RTPipelineExtensions = {
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME
        };

        // Choosing a GPU based on its features, GPUs with RT pipeline are favoured the most
        for (const auto& dev : availableDevices)
        {
            if (!IsDeviceSuitable(dev.PhysicalDevice, surface)) continue;

            int32_t currentScore = 0;

            if (dev.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                currentScore += 1000;

            if (CheckPhysicalDeviceExtensionsSupport(dev.PhysicalDevice, RTPipelineExtensions))
                currentScore += 10000;

            if (currentScore > maxScore)
            {
                maxScore = currentScore;
                chosenDevice = dev;
            }
        }

        return chosenDevice;
    }

#pragma region PhysicalDeviceChecks
    bool VK_Device::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // Checks if all minimal requirements are satisfied by a device

        const auto indices = FindQueueFamilies(device, surface);
        const auto areExtensionsSupported = CheckPhysicalDeviceExtensionsSupport(device, VK_Config::RequiredPhysicalDeviceExtensions);
        const auto swapchainSupportDetails = QuerySwapchainSupportDetails(device, surface);

        return indices.IsComplete() && areExtensionsSupported && swapchainSupportDetails.IsSupportAdequate();
    }

    bool VK_Device::CheckPhysicalDeviceExtensionsSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions)
    {
        // Checks if all specified extensions are supported by a device
        // returns false if at least one extension is not supported

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        auto availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

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