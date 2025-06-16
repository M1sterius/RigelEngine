#pragma once

#include "Core.hpp"
#include "vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Instance
    {
    public:
        explicit VK_Instance();
        ~VK_Instance();

        VK_Instance(const VK_Instance&) = delete;
        VK_Instance operator = (const VK_Instance&) = delete;

        NODISCARD VkInstance Get() const { return m_Instance; }
    private:
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        std::vector<VkExtensionProperties> m_SupportedExtensions;

        NODISCARD static uint32_t GetVulkanVersion();

        /**
         * Gets all extensions available on this device and checks if requested extensions are among them.
         * If at least one extension is not supported, returns false.
         *
         * @param extensionsToEnable Puts all extensions that were requested and now have to be enabled into this vector
         */
        NODISCARD static bool CheckExtensionsSupport(std::vector<const char*>& extensionsToEnable);
        NODISCARD static bool CheckValidationLayersSupport();

        void CreateDebugMessenger();
        void DestroyDebugMessenger();
    };
}
