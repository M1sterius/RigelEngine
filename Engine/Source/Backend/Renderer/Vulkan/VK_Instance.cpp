#include "VK_Instance.hpp"
#include "VK_Config.hpp"
#include "Debug.hpp"

namespace rge::backend
{

    VK_Instance::VK_Instance()
    {
        // Get all supported extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        m_SupportedExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_SupportedExtensions.data());

        for (const auto& extension : m_SupportedExtensions)
            rge::Debug::Message(extension.extensionName);
    }

    VK_Instance::~VK_Instance()
    {

    }
}