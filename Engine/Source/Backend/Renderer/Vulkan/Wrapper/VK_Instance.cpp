#include "VK_Instance.hpp"
#include "VK_Config.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "VulkanException.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

#include <format>
#include <cstring>

namespace Rigel::Backend::Vulkan
{
#pragma region VulkanDebugMessanger

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        Debug::Error(std::format("validation layer: {}", pCallbackData->pMessage));
        return VK_FALSE;
    }

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debug_callback;
    }

#pragma endregion

    VK_Instance::VK_Instance()
    {
        Debug::Trace("Creating Vulkan instance.");

        if (!CheckVulkanVersionSupport(VK_Config::MinimalRequiredVulkanVersion))
            throw RigelException("Minimal required vulkan version is not supported on this device!");

        if (VK_Config::EnableValidationLayers && !CheckValidationLayersSupport())
            throw RigelException("Validation layers requested, but not available!");

        auto extensionsToEnable = std::vector<const char*>();
        if (!CheckExtensionsSupport(extensionsToEnable))
            throw RigelException("Not all requested vulkan instance extensions are supported on this device!");

        auto appInfo = MakeInfo<VkApplicationInfo>();
        appInfo.pApplicationName = "Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = RIGEL_ENGINE_NAME;
        appInfo.engineVersion = RGE_API_CURRENT_VERSION;
        appInfo.apiVersion = VK_Config::MinimalRequiredVulkanVersion;

        auto createInfo = MakeInfo<VkInstanceCreateInfo>();
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionsToEnable.size();
        createInfo.ppEnabledExtensionNames = extensionsToEnable.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (VK_Config::EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VK_Config::ValidationLayers.size());
            createInfo.ppEnabledLayerNames = VK_Config::ValidationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (const auto result = vkCreateInstance(&createInfo, nullptr, &m_Instance); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan instance!", result);

        if (VK_Config::EnableValidationLayers)
            CreateDebugMessenger();
    }

    VK_Instance::~VK_Instance()
    {
        if (VK_Config::EnableValidationLayers)
            DestroyDebugMessenger();
        vkDestroyInstance(m_Instance, nullptr);

        Debug::Trace("Vulkan instance destroyed.");
    }

    bool VK_Instance::CheckVulkanVersionSupport(const uint32_t version)
    {
        uint32_t supportedVersion = 0;

        if (vkEnumerateInstanceVersion)
            vkEnumerateInstanceVersion(&supportedVersion);

        if (supportedVersion < version)
            return false;

        return true;
    }

    bool VK_Instance::CheckExtensionsSupport(std::vector<const char*>& extensionsToEnable)
    {
        auto supportedExtensions = std::vector<VkExtensionProperties>();

        // Get all extensions supported on this device
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        if (extensionCount == 0)
            throw RigelException("Failed to find any supported instance extensions.");

        supportedExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

        // Get all extensions required by glfw
        uint32_t glfwRequiredExtensionsCount = 0;
        const auto glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionsCount);

        if (glfwRequiredExtensionsCount == 0)
            throw RigelException("glfwGetRequiredInstanceExtensions results 0 as number of required instance extensions.");

        // Combine all requested extensions into one vector
        auto requiredExtensions = VK_Config::RequiredInstanceExtensions;
        for (uint32_t i = 0; i < glfwRequiredExtensionsCount; i++)
            requiredExtensions.push_back(glfwRequiredExtensions[i]);

        // Add validation layers extension to the required extensions vector if validation layers are enabled
        if (VK_Config::EnableValidationLayers)
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        extensionsToEnable = requiredExtensions;

        // Check if all required extensions exist in the set of all supported extensions
        for (const char* requiredExt : requiredExtensions)
        {
            bool found = false;

            for (const auto& availableExt : supportedExtensions)
            {
                if (strcmp(availableExt.extensionName, requiredExt) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                Debug::Error(std::format("Requested Vulkan extension {} is not supported on this device!", requiredExt));
                return false;  // At least one required extension is missing
            }
        }

        return true; // All required extensions are supported
    }

    bool VK_Instance::CheckValidationLayersSupport()
    {
        auto availableLayers = std::vector<VkLayerProperties>();

        // Get all supported instance layers
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

        if (availableLayerCount == 0)
            Debug::Warning("No vulkan instance layers available.");

        availableLayers.resize(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        for (const char* layerName : VK_Config::ValidationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VK_Instance::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        if (const auto result = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger); result != VK_SUCCESS)
            throw VulkanException("Failed to set up Vulkan debug messenger!", result);
    }

    void VK_Instance::DestroyDebugMessenger()
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
}
