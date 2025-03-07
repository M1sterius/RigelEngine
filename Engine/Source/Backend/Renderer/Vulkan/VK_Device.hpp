#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <utility>

namespace rge::backend
{
    struct PhysicalDeviceInfo
    {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties Properties = {};

        NODISCARD bool IsNull() const
        {
            return PhysicalDevice == VK_NULL_HANDLE;
        }
    };

    struct QueueFamilyIndices
    {
        // If a device does not support a queue family, the corresponding std::optional won't have a value

        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        NODISCARD inline bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities {};
        std::vector<VkSurfaceFormatKHR> Formats {};
        std::vector<VkPresentModeKHR> PresentModes {};

        NODISCARD inline bool IsSupportAdequate() const
        {
            return !Formats.empty() && !PresentModes.empty();
        }
    };

    class VK_Device
    {
    public:
        VK_Device(VkInstance instance, VkSurfaceKHR surface);
        ~VK_Device();

        VK_Device(const VK_Device&) = delete;
        VK_Device operator = (const VK_Device) = delete;
    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        PhysicalDeviceInfo m_SelectedPhysicalDevice = {};
        VkDevice m_Device = VK_NULL_HANDLE;

        void CreateLogicalDevice();

        NODISCARD static std::vector<PhysicalDeviceInfo> FindPhysicalDevices(VkInstance instance);
        NODISCARD static PhysicalDeviceInfo PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface);

        NODISCARD static bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static bool CheckPhysicalDeviceExtensionsSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions);
        NODISCARD static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static SwapChainSupportDetails QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);
    };
}
