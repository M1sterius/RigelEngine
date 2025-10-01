#pragma once

#include "Core.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Swapchain;
    class VK_MemoryBuffer;
    class VK_DescriptorSet;
    class VK_DescriptorPool;

    struct SceneData;

    class VK_GPUScene
    {
    public:
        VK_GPUScene(VK_Device& device, VK_Swapchain& swapchain);
        ~VK_GPUScene();

        VK_GPUScene(const VK_GPUScene&) = delete;
        VK_GPUScene operator = (const VK_GPUScene&) = delete;
    private:
        VK_Device& m_Device;

        std::unique_ptr<SceneData> m_SceneData;

        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::vector<VkDescriptorSet> m_DescriptorSets;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;
        std::vector<std::unique_ptr<VK_MemoryBuffer>> m_SceneDataBuffers;
    };
}
