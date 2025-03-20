#include "VK_Device.hpp"
#include "VK_Config.hpp"
#include "VulkanException.hpp"
#include "MakeInfo.hpp"

#include <format>
#include <set>

namespace rge::backend
{
    VK_Device::VK_Device(VkInstance instance, VkSurfaceKHR surface)
        : m_Instance(instance), m_Surface(surface)
    {
        Debug::Trace("Searching for GPUs with Vulkan support.");
        auto availableDevices = FindPhysicalDevices(m_Instance);
        Debug::Trace("Detected GPUs:");
        for (const auto& device : availableDevices)
            Debug::Trace(std::format("    {}", device.Properties.deviceName));

        m_SelectedPhysicalDevice = PickBestPhysicalDevice(availableDevices, m_Surface);
        if (m_SelectedPhysicalDevice.IsNull())
            throw RigelException("Failed to find any GPUs with adequate support of required features!");

        m_QueueFamilyIndices = FindQueueFamilies(m_SelectedPhysicalDevice.PhysicalDevice, surface);

        Debug::Trace(std::format("Selected GPU: {}.", m_SelectedPhysicalDevice.Properties.deviceName));

        CreateLogicalDevice();
        CreateCommandPool();
    }

    VK_Device::~VK_Device()
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyDevice(m_Device, nullptr);

        Debug::Trace("Vulkan device destroyed.");
    }

    void VK_Device::CreateLogicalDevice()
    {
        Debug::Trace("Creating logical Vulkan device.");

        const auto indices = FindQueueFamilies(m_SelectedPhysicalDevice.PhysicalDevice, m_Surface);

        auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>();

        // Eliminates duplicates if a single queue supports multiple operations
        std::set<uint32_t> uniqueQueueFamilies = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

        float queuePriority = 1.0f; // Priorities only matter when creating multiple queues within a single family
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            auto queueCreateInfo = MakeInfo<VkDeviceQueueCreateInfo>();
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Put physical device features you want to be enabled here
        VkPhysicalDeviceFeatures deviceFeatures {};
        deviceFeatures.samplerAnisotropy = true;

        auto createInfo = MakeInfo<VkDeviceCreateInfo>();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(VK_Config::RequiredPhysicalDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = VK_Config::RequiredPhysicalDeviceExtensions.data();

        if (VK_Config::EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VK_Config::ValidationLayers.size());
            createInfo.ppEnabledLayerNames = VK_Config::ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (const auto result = vkCreateDevice(m_SelectedPhysicalDevice.PhysicalDevice, &createInfo, nullptr, &m_Device); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan logical device", result);

        vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
    }

    void VK_Device::CreateCommandPool()
    {
        // TODO: Implement creating command pool for each thread or for each queue family

        auto poolCreateInfo = MakeInfo<VkCommandPoolCreateInfo>();
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();

        if (const auto result = vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &m_CommandPool); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan command pool!", result);
    }

    uint32_t VK_Device::FindMemoryType(const uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        const auto memProperties = m_SelectedPhysicalDevice.MemoryProperties;

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        throw VulkanException("Failed to find suitable memory type supported by a Vulkan physical device!", VK_ERROR_UNKNOWN);
    }

    void VK_Device::WaitIdle() const
    {
        vkDeviceWaitIdle(m_Device);
    }

    std::vector<PhysicalDeviceInfo> VK_Device::FindPhysicalDevices(VkInstance instance)
    {
        auto devices = std::vector<VkPhysicalDevice>();

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw RigelException("Failed to find GPUs with Vulkan support!");

        devices.resize(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        auto devicesInfo = std::vector<PhysicalDeviceInfo>(deviceCount);

        // Retrieve the properties of all detected devices
        for (size_t i = 0; i < devices.size(); i++)
        {
            const auto device = devices[i];
            devicesInfo[i].PhysicalDevice = device;

            vkGetPhysicalDeviceProperties(device, &devicesInfo[i].Properties);
            vkGetPhysicalDeviceMemoryProperties(device, &devicesInfo[i].MemoryProperties);
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
        // Searches for support of graphics and present queue families on a device,
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

    SwapchainSupportDetails VK_Device::QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // Retrieve information about swap chain support details on a device

        SwapchainSupportDetails details;

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
