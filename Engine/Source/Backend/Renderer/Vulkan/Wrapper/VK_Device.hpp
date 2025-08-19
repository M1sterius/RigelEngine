#pragma once

#include "Core.hpp"
#include "QueueType.hpp"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace Rigel::Backend::Vulkan
{
    class VK_MemoryBuffer;
    class VK_CmdPool;

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
        std::optional<uint32_t> TransferFamily;

        NODISCARD inline bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value() && TransferFamily.has_value();
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

        // Thread safe way to submit work to a queue, always prefer it to raw vkQueueSubmit
        void SubmitToQueue(const QueueType queueType, const uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const;
        NODISCARD VkResult SubmitPresentQueue(const VkPresentInfoKHR* pPresentInfo) const;

        NODISCARD VK_CmdPool& GetCommandPool(const QueueType queueType) const;

        NODISCARD inline VmaAllocator GetVmaAllocator() const { return m_VmaAllocator; }
        NODISCARD inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        NODISCARD inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
        NODISCARD inline VkQueue GetTransferQueue() const { return m_TransferQueue; }

        NODISCARD inline const PhysicalDeviceInfo& GetPhysicalDevice() const { return m_SelectedPhysicalDevice; }
        NODISCARD inline const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
        NODISCARD inline const SwapchainSupportDetails& GetSwapchainSupportDetails() const { return m_SwapchainSupportDetails; }

        void WaitIdle() const;
    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkDevice m_Device = VK_NULL_HANDLE;
        VmaAllocator m_VmaAllocator = VK_NULL_HANDLE;

        QueueFamilyIndices m_QueueFamilyIndices = {};
        PhysicalDeviceInfo m_SelectedPhysicalDevice = {};
        SwapchainSupportDetails m_SwapchainSupportDetails = {};

        mutable std::mutex m_GraphicsQueueMutex;
        mutable std::mutex m_TransferQueueMutex;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        VkQueue m_TransferQueue = VK_NULL_HANDLE;

        std::vector<const char*> m_EnabledExtensions;

        void CreateLogicalDevice();
        void CreateVmaAllocator();
        void CreateCommandPools();

        std::unordered_map<std::thread::id, std::unordered_map<QueueType, std::unique_ptr<VK_CmdPool>>> m_CommandPools;

        NODISCARD static std::vector<PhysicalDeviceInfo> FindPhysicalDevices(VkInstance instance);
        NODISCARD static PhysicalDeviceInfo PickBestPhysicalDevice(const std::vector<PhysicalDeviceInfo>& availableDevices, VkSurfaceKHR surface);

        NODISCARD static bool IsDeviceSuitable(const PhysicalDeviceInfo& device, VkSurfaceKHR surface);
        NODISCARD static bool IsPhysicalDeviceExtensionSupported(const PhysicalDeviceInfo& device, const char* extName);
        NODISCARD static bool CheckPhysicalDeviceExtensionsSupport(const PhysicalDeviceInfo& device, const std::vector<const char*>& extensions);
        NODISCARD static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        NODISCARD static SwapchainSupportDetails QuerySwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);
    };
}
