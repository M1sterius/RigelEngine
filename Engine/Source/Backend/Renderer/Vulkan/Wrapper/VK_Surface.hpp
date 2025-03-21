#pragma once

#include "Core.hpp"
#include "vulkan.h"

namespace rge::backend
{
    class VK_Surface
    {
    public:
        explicit VK_Surface(VkInstance instance);
        ~VK_Surface();

        VK_Surface(const VK_Surface&) = delete;
        VK_Surface operator = (const VK_Surface) = delete;

        NODISCARD inline VkSurfaceKHR Get() const { return m_Surface; }
    private:
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkInstance m_Instance;
    };
}