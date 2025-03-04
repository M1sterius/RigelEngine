#include "VK_Instance.hpp"
#include "VK_Config.hpp"
#include "Debug.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

namespace rge::backend
{
    VK_Instance::VK_Instance()
    {
        if (!CheckVulkanVersionSupport(MinimalRequiredVulkanVersion))
        {
            THROW_RUNTIME_ERROR("Minimal required vulkan version is not supported on this device!");
        }

        uint32_t glfwExtensionCount = 0;
        const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (glfwExtensionCount == 0)
        {
            THROW_RUNTIME_ERROR("glfwGetRequiredInstanceExtensions results 0 as number of required instance extensions.");
        }

        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            Debug::Message(glfwExtensions[i]);
        }
    }

    VK_Instance::~VK_Instance()
    {

    }

    std::vector<VkExtensionProperties> VK_Instance::GetSupportedExtensions()
    {
        auto extensions = std::vector<VkExtensionProperties>();

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        if (extensionCount == 0)
        {
            THROW_RUNTIME_ERROR("Failed to find any supported instance extensions.");
        }

        extensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    std::vector<VkLayerProperties> VK_Instance::GetSupportedLayers()
    {
        auto layers = std::vector<VkLayerProperties>();

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        if (layerCount == 0)
            Debug::Warning("No vulkan instance layers available.");

        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        return layers;
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
}
