#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

namespace Rigel::Backend
{
    struct SceneRenderInfo;
}

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
    class VK_BindlessManager;
    class VK_GPUScene;

    struct MeshData;

    class VK_GeometryPass
    {
    public:
        VK_GeometryPass(VK_Device& device, VK_Swapchain& swapchain, VK_BindlessManager& bindlessManager,
            VK_GBuffer& gBuffer, VK_GPUScene& gpuScene);
        ~VK_GeometryPass();

        VK_GeometryPass(const VK_GeometryPass&) = delete;
        VK_GeometryPass operator = (const VK_GeometryPass&) = delete;

        NODISCARD VkCommandBuffer RecordCommandBuffer(const uint32_t frameIndex);
    private:
        VK_Device& m_Device;
        VK_Swapchain& m_Swapchain;
        VK_BindlessManager& m_BindlessManager;
        VK_GBuffer& m_GBuffer;
        VK_GPUScene& m_GPUScene;

        static constexpr uint32_t MAX_MESHES = 2048;

        void CreateGraphicsPipeline();

        std::unique_ptr<VK_GraphicsPipeline> m_GraphicsPipeline;
        std::vector<std::unique_ptr<VK_CmdBuffer>> m_CommandBuffers;
    };
}
