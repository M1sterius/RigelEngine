#include "VK_GPUScene.hpp"
#include "Wrapper/VulkanWrapper.hpp"
#include "../ShaderStructs.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GPUScene::VK_GPUScene(VK_Device& device, VK_Swapchain& swapchain)
        : m_Device(device), m_DescriptorSetLayout(nullptr)
    {

    }

    VK_GPUScene::~VK_GPUScene()
    {

    }
}
