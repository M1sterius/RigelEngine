#include "VK_LightingPass.hpp"
#include "VulkanWrapper.hpp"
#include "VulkanUtility.hpp"
#include "ShaderStructs.hpp"
#include "VK_BindlessManager.hpp"
#include "VK_GBuffer.hpp"
#include "Assets/Model.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/Renderer/SceneRenderInfo.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_LightingPass::VK_LightingPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer)
        : m_Device(device), m_Swapchain(swapchain), m_GBuffer(gBuffer)
    {
        
    }

    VK_LightingPass::~VK_LightingPass()
    {

    }
}
