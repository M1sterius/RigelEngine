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

    struct SwapchainSupportDetails
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

        NODISCARD SwapchainSupportDetails GetSwapchainSupportDetails() const { return m_SwapchainSupportDetails; }
    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkDevice m_Device = VK_NULL_HANDLE;
        PhysicalDeviceInfo m_SelectedPhysicalDevice = {};
        QueueFamilyIndices m_QueueFamilyIndices = {};
        SwapchainSupportDetails m_SwapchainSupportDetails;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        void CreateLogicalDevice();
        void CreateCommandPool();

        NODISCARD static std::vector<PhysicalDeviceInfo> FindPhysicalDevices(VkInstance instance);
        NODISCARD static PhysicalDeviceInfo PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface);

        NODISCARD static bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static bool CheckPhysicalDeviceExtensionsSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions);
        NODISCARD static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static SwapchainSupportDetails QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);
    };
}
