#pragma once

#include "Core.hpp"

#include "vulkan.h"

namespace rge::backend
{
    class VK_Device;

    class VK_Image 
    {
    public:
        static void TransitionLayout(VK_Device& device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    };

}
