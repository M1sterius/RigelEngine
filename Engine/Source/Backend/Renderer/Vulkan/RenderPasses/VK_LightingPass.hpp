#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_GBuffer;
    class VK_GraphicsPipeline;
    class VK_CmdBuffer;
    class VK_MemoryBuffer;
    class VK_VertexBuffer;
    class VK_IndexBuffer;
    class VK_DescriptorPool;
    class VK_Swapchain;

    class VK_LightingPass 
    {
    public:
        VK_LightingPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer);
        ~VK_LightingPass();

        VK_LightingPass(const VK_LightingPass&) = delete;
        VK_LightingPass operator = (const VK_LightingPass&) = delete;
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;
        VK_GBuffer& m_GBuffer;
    };
}
