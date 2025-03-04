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

        std::vector<VkExtensionProperties> m_SupportedExtensions;
    };
}
