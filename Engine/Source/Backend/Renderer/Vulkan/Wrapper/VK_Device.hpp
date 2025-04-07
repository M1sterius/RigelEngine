#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <utility>

namespace Rigel::Backend
{
    struct PhysicalDeviceInfo
    {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties Properties = {};
        VkPhysicalDeviceMemoryProperties MemoryProperties {};

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

        NODISCARD inline VkDevice Get() const { return m_Device; }

        NODISCARD inline SwapchainSupportDetails GetSwapchainSupportDetails() const { return QuerySwapchainSupportDetails(m_SelectedPhysicalDevice.PhysicalDevice, m_Surface); }
        NODISCARD inline QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

        NODISCARD inline VkCommandPool GetCommandPool() const { return m_CommandPool; }

        NODISCARD inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        NODISCARD inline VkQueue GetPresentQueue() const { return m_PresentQueue; }

        NODISCARD uint32_t FindMemoryType(const uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        void WaitIdle() const;
    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkDevice m_Device = VK_NULL_HANDLE;
        PhysicalDeviceInfo m_SelectedPhysicalDevice = {};
        QueueFamilyIndices m_QueueFamilyIndices = {};

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
