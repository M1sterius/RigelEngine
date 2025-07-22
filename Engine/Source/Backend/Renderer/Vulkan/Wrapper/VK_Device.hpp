#pragma once

#include "Core.hpp"
#include "vulkan.h"
#include "vk_mem_alloc.h"

#include <utility>
#include <mutex>

namespace Rigel::Backend::Vulkan
{
    class VK_MemoryBuffer;

    struct PhysicalDeviceInfo
    {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties Properties = {};
        VkPhysicalDeviceMemoryProperties MemoryProperties {};
        VkDeviceSize DedicatedMemorySize;
        std::vector<VkExtensionProperties> SupportedExtensions;

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

        NODISCARD inline const PhysicalDeviceInfo& GetPhysicalDevice() const { return m_SelectedPhysicalDevice; }
        NODISCARD inline VmaAllocator GetVmaAllocator() const { return m_VmaAllocator; }
        NODISCARD inline SwapchainSupportDetails GetSwapchainSupportDetails() const { return QuerySwapchainSupportDetails(m_SelectedPhysicalDevice.PhysicalDevice, m_Surface); }
        NODISCARD inline QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

        NODISCARD VkCommandPool GetCommandPool() const;
        NODISCARD VK_MemoryBuffer& GetStagingBuffer() const;

        // thread safe way to submit work to the graphics queue, always prefer it to raw vkQueueSubmit
        void SubmitGraphicsQueue(const uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const;
        NODISCARD VkResult SubmitPresentQueue(const VkPresentInfoKHR* pPresentInfo);

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
        VmaAllocator m_VmaAllocator = VK_NULL_HANDLE;

        mutable std::mutex m_GraphicsQueueMutex;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        std::vector<const char*> m_EnabledExtensions;
        std::unordered_map<std::thread::id, VkCommandPool> m_CommandPools;
        std::unordered_map<std::thread::id, std::unique_ptr<VK_MemoryBuffer>> m_StagingBuffers;

        void CreateLogicalDevice();
        void CreateVmaAllocator();
        void CreateCommandPools();
        void CreateStagingBuffers();

        NODISCARD static std::vector<PhysicalDeviceInfo> FindPhysicalDevices(VkInstance instance);
        NODISCARD static PhysicalDeviceInfo PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface);

        NODISCARD static bool IsDeviceSuitable(const PhysicalDeviceInfo& device, VkSurfaceKHR surface);
        NODISCARD static bool IsPhysicalDeviceExtensionSupported(const PhysicalDeviceInfo& device, const char* extName);
        NODISCARD static bool CheckPhysicalDeviceExtensionsSupport(const PhysicalDeviceInfo& device, const std::vector<const char*>& extensions);
        NODISCARD static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static SwapchainSupportDetails QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);
    };
}
