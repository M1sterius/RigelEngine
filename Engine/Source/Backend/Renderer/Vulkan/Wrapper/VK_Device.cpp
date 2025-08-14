#include "VK_Device.hpp"
#include "VulkanUtility.hpp"
#include "VK_MemoryBuffer.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

#include <format>
#include <set>

namespace Rigel::Backend::Vulkan
{
    VK_Device::VK_Device(VkInstance instance, VkSurfaceKHR surface)
        : m_Instance(instance), m_Surface(surface)
    {
        Debug::Trace("Searching for GPUs with Vulkan support.");
        const auto availableDevices = FindPhysicalDevices(m_Instance);
        Debug::Trace("Detected GPUs:");
        for (const auto& device : availableDevices)
            Debug::Trace(std::format("    {}", device.Properties.deviceName));

        m_SelectedPhysicalDevice = PickBestPhysicalDevice(availableDevices, m_Surface);
        if (m_SelectedPhysicalDevice.IsNull())
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Failed to find any GPUs with adequate support of required vulkan features!", __FILE__, __LINE__);
        }

        m_QueueFamilyIndices = FindQueueFamilies(m_SelectedPhysicalDevice.PhysicalDevice, surface);

        Debug::Trace("Selected GPU: {}.", m_SelectedPhysicalDevice.Properties.deviceName);
        Debug::Trace("Available dedicated VRAM: {}mb.", m_SelectedPhysicalDevice.DedicatedMemorySize / (1024 * 1024));

        CreateLogicalDevice();
        CreateVmaAllocator();
        CreateCommandPools();
        CreateStagingBuffers();
    }

    VK_Device::~VK_Device()
    {
        Debug::Trace("Destroying vulkan device.");

        m_StagingBuffers.clear();

        vmaDestroyAllocator(m_VmaAllocator);

        for (const auto& pools : m_CommandPools | std::views::values)
        {
            for (const auto pool : pools | std::views::values)
                vkDestroyCommandPool(m_Device, pool, nullptr);
        }

        vkDestroyDevice(m_Device, nullptr);
    }

    void VK_Device::CreateLogicalDevice()
    {
        Debug::Trace("Creating logical Vulkan device.");

        auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>();

        // Eliminates duplicates if a single queue supports multiple operations
        std::set uniqueQueueFamilies = {
            m_QueueFamilyIndices.GraphicsFamily.value(),
            m_QueueFamilyIndices.PresentFamily.value(),
            m_QueueFamilyIndices.TransferFamily.value()
        };

        float queuePriority = 1.0f; // Priorities only matter when creating multiple queues within the same family
        for (const auto queueFamily : uniqueQueueFamilies)
        {
            auto queueCreateInfo = MakeInfo<VkDeviceQueueCreateInfo>();
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        m_EnabledExtensions = VK_Config::RequiredPhysicalDeviceExtensions;

        // Add supported optional extensions to be enabled
        for (const auto extName : VK_Config::OptionalPhysicalDeviceExtensions)
        {
            if (IsPhysicalDeviceExtensionSupported(m_SelectedPhysicalDevice, extName))
                m_EnabledExtensions.push_back(extName);
            else
                Debug::Warning("Optional vulkan physical device extension {} is not supported.", extName);
        }

        Debug::Trace("Enabled vulkan physical device extensions:");
        for (const auto ext : m_EnabledExtensions)
            Debug::Trace("    {}", ext);

        // Put physical device features you want to be enabled here
        VkPhysicalDeviceFeatures deviceFeatures {};
        deviceFeatures.samplerAnisotropy = true;

        // Enable bindless descriptors (REQUIRED)
        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
        indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
        indexingFeatures.runtimeDescriptorArray = VK_TRUE;
        indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        indexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
        indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;

        // Enable dynamic rendering (REQUIRED)
        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature {};
        dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRenderingFeature.dynamicRendering = VK_TRUE;

        // Enable buffer device address (OPTIONAL)
        if (IsPhysicalDeviceExtensionSupported(m_SelectedPhysicalDevice, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
        {
            VkPhysicalDeviceBufferDeviceAddressFeatures bufferAddressFeatures {};
            bufferAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
            bufferAddressFeatures.bufferDeviceAddress = VK_TRUE;

            dynamicRenderingFeature.pNext = &bufferAddressFeatures;
        }

        indexingFeatures.pNext = &dynamicRenderingFeature;

        auto createInfo = MakeInfo<VkDeviceCreateInfo>();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_EnabledExtensions.size());
        createInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();
        createInfo.pNext = &indexingFeatures;

        if (VK_Config::EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VK_Config::ValidationLayers.size());
            createInfo.ppEnabledLayerNames = VK_Config::ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        VK_CHECK_RESULT(vkCreateDevice(m_SelectedPhysicalDevice.PhysicalDevice, &createInfo, nullptr, &m_Device), "Failed to create logical device!");

        vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.PresentFamily.value(), 0, &m_PresentQueue);
        vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.TransferFamily.value(), 0, &m_TransferQueue);
    }

    void VK_Device::CreateVmaAllocator()
    {
        static const std::unordered_map<std::string, VmaAllocatorCreateFlags> extensionToFlagMap = {
            {"VK_KHR_dedicated_allocation",       VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT},
            {"VK_KHR_bind_memory2",               VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT},
            {"VK_KHR_maintenance4",               VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT},
            {"VK_KHR_maintenance5",               VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT},
            {"VK_EXT_memory_budget",              VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT},
            {"VK_KHR_buffer_device_address",      VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT},
            {"VK_EXT_memory_priority",            VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT},
            {"VK_AMD_device_coherent_memory",     VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT},
            {"VK_KHR_external_memory_win32",      VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT}
        };

        VmaAllocatorCreateFlags extensionFlags = 0;
        for (const auto ext : m_EnabledExtensions)
        {
            if (extensionToFlagMap.contains(ext))
                extensionFlags |= extensionToFlagMap.at(ext);
        }

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = extensionFlags;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_4;
        allocatorCreateInfo.physicalDevice = m_SelectedPhysicalDevice.PhysicalDevice;
        allocatorCreateInfo.device = m_Device;
        allocatorCreateInfo.instance = m_Instance;

        VK_CHECK_RESULT(vmaCreateAllocator(&allocatorCreateInfo, &m_VmaAllocator), "Failed to create VMA allocator object!");
    }

    void VK_Device::CreateCommandPools()
    {
        VkCommandPool commandPool = VK_NULL_HANDLE;

        auto poolCreateInfo = MakeInfo<VkCommandPoolCreateInfo>();
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        // Graphics queue
        poolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();

        VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &commandPool), "Failed to create command pool!");
        m_CommandPools[std::this_thread::get_id()][QueueType::Graphics] = commandPool;

        for (const auto& threadId : GetAssetManager()->GetLoadingThreadIDs())
        {
            VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &commandPool), "Failed to create command pool!");
            m_CommandPools[threadId][QueueType::Graphics] = commandPool;
        }

        // Transfer queue
        poolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.TransferFamily.value();

        VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &commandPool), "Failed to create command pool!");
        m_CommandPools[std::this_thread::get_id()][QueueType::Transfer] = commandPool;

        for (const auto& threadId : GetAssetManager()->GetLoadingThreadIDs())
        {
            VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &commandPool), "Failed to create command pool!");
            m_CommandPools[threadId][QueueType::Transfer] = commandPool;
        }
    }

    void VK_Device::CreateStagingBuffers()
    {
        m_StagingBuffers[std::this_thread::get_id()] = std::make_unique<VK_MemoryBuffer>(*this, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);

        for (const auto& id : GetAssetManager()->GetLoadingThreadIDs())
        {
            m_StagingBuffers[id] = std::make_unique<VK_MemoryBuffer>(*this, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
        }
    }

    VkCommandPool VK_Device::GetCommandPool(const QueueType queueType) const
    {
        const auto thisThreadID = std::this_thread::get_id();
        if (!m_CommandPools.contains(thisThreadID))
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Command pool can only be retrieved for one of the asset manager's loading threads or the main thread!", __FILE__, __LINE__);
        }

        return m_CommandPools.at(thisThreadID).at(queueType);
    }

    VK_MemoryBuffer& VK_Device::GetStagingBuffer() const
    {
        const auto thisThreadID = std::this_thread::get_id();
        if (!m_StagingBuffers.contains(thisThreadID))
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Staging buffer can only be retrieved for one of the asset manager's loading threads or the main thread!", __FILE__, __LINE__);
        }

        return *m_StagingBuffers.at(thisThreadID);
    }

    void VK_Device::SubmitToQueue(const QueueType queueType, const uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const
    {
        if (queueType == QueueType::Graphics)
        {
            std::unique_lock lock(m_GraphicsQueueMutex);
            VK_CHECK_RESULT(vkQueueSubmit(GetGraphicsQueue(), submitCount, submitInfo, fence), "Failed to submit a command buffer to the graphics queue!");
        }
        else if (queueType == QueueType::Transfer)
        {
            std::unique_lock lock(m_TransferQueueMutex);
            VK_CHECK_RESULT(vkQueueSubmit(GetTransferQueue(), submitCount, submitInfo, fence), "Failed to submit a command buffer to the transfer queue!");
        }
    }

    VkResult VK_Device::SubmitPresentQueue(const VkPresentInfoKHR* pPresentInfo) const
    {
        std::unique_lock lock(m_GraphicsQueueMutex);
        return vkQueuePresentKHR(m_PresentQueue, pPresentInfo);
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
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Failed to find GPUs with Vulkan support!", __FILE__, __LINE__);
        }

        devices.resize(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        auto devicesInfo = std::vector<PhysicalDeviceInfo>(deviceCount);

        // Retrieve the properties of all detected devices
        for (size_t i = 0; i < devices.size(); i++)
        {
            const auto device = devices[i];
            auto& deviceInfo = devicesInfo[i];

            deviceInfo.PhysicalDevice = device;

            vkGetPhysicalDeviceProperties(device, &deviceInfo.Properties);
            vkGetPhysicalDeviceMemoryProperties(device, &deviceInfo.MemoryProperties);

            // Get all extensions supported by a device
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            deviceInfo.SupportedExtensions.resize(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceInfo.SupportedExtensions.data());

            // Query total device VRAM
            for (const auto memoryHeap : deviceInfo.MemoryProperties.memoryHeaps)
            {
                if (memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    deviceInfo.DedicatedMemorySize += memoryHeap.size;
            }
        }

        return devicesInfo;
    }

    PhysicalDeviceInfo VK_Device::PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface)
    {
        // If there is only one GPU we don't have to do anything besides checking it for minimal requirements
        if (availableDevices.size() == 1)
            return IsDeviceSuitable(availableDevices[0], surface) ? availableDevices[0] : PhysicalDeviceInfo();

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
        for (const auto& device : availableDevices)
        {
            if (!IsDeviceSuitable(device, surface))
                continue;

            int32_t currentScore = 0;

            if (device.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                currentScore += 1000;

            if (CheckPhysicalDeviceExtensionsSupport(device, RTPipelineExtensions))
                currentScore += 10000;

            if (currentScore > maxScore)
            {
                maxScore = currentScore;
                chosenDevice = device;
            }
        }

        return chosenDevice;
    }

#pragma region PhysicalDeviceChecks
    bool VK_Device::IsDeviceSuitable(const PhysicalDeviceInfo& device, VkSurfaceKHR surface)
    {
        // Checks if all minimal requirements are satisfied by a device

        const auto indices = FindQueueFamilies(device.PhysicalDevice, surface);
        const auto areExtensionsSupported = CheckPhysicalDeviceExtensionsSupport(device, VK_Config::RequiredPhysicalDeviceExtensions);
        const auto swapchainSupportDetails = QuerySwapchainSupportDetails(device.PhysicalDevice, surface);

        return indices.IsComplete() && areExtensionsSupported && swapchainSupportDetails.IsSupportAdequate();
    }

    bool VK_Device::IsPhysicalDeviceExtensionSupported(const PhysicalDeviceInfo& device, const char* extName)
    {
        for (const auto [curExtName, _] : device.SupportedExtensions)
        {
            if (strcmp(curExtName, extName) == 0)
                return true;
        }

        return false;
    }

    bool VK_Device::CheckPhysicalDeviceExtensionsSupport(const PhysicalDeviceInfo& device, const std::vector<const char*>& extensions)
    {
        // Checks if all specified extensions are supported by a device
        // returns false if at least one extension is not supported

        std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

        for (const auto& [extensionName, _] : device.SupportedExtensions)
            requiredExtensions.erase(extensionName);

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VK_Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // Searches for support of graphics, transfer and present queue families on a device,
        // corresponding std::optional in QueueFamilyIndices won't have a value if the queue isn't supported

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && !indices.GraphicsFamily.has_value())
                indices.GraphicsFamily = i;

            // dedicated transfer queue
            if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                indices.TransferFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport && !indices.PresentFamily.has_value())
                indices.PresentFamily = i;

            if (indices.IsComplete())
                break;

            ++i;
        }

        // no dedicated transfer queue, so we have to use graphics queue instead
        if (!indices.TransferFamily.has_value())
            indices.TransferFamily = indices.GraphicsFamily;

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
