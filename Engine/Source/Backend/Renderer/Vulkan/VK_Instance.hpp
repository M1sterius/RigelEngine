#pragma once

#include "vulkan.h"

#include <vector>

namespace rge::backend
{
    class VK_Instance
    {
    public:
        explicit VK_Instance();
        ~VK_Instance();

        VK_Instance(const VK_Instance&) = delete;
        VK_Instance operator = (const VK_Instance&) = delete;

        NODISCARD operator VkInstance() const { return m_Instance; } // implicit conversion intended
    private:
        VkInstance m_Instance = VK_NULL_HANDLE;

        NODISCARD static std::vector<VkExtensionProperties> GetSupportedExtensions();
        NODISCARD static std::vector<VkLayerProperties> GetSupportedLayers();

        // Will return true if vulkan version supported on this device is greater or equal to the specified version
        NODISCARD static bool CheckVulkanVersionSupport(const uint32_t version);
    };
}
