#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

#include <memory>
#include <array>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_Image;
    class VK_DescriptorPool;

    class VK_GBuffer
    {
    public:
        static constexpr VkFormat POSITION_ATTACHMENT_FORMAT = VK_FORMAT_R32G32B32A32_SFLOAT;
        static constexpr VkFormat NORMAL_ATTACHMENT_FORMAT = VK_FORMAT_R32G32B32A32_SFLOAT;
        static constexpr VkFormat ALBEDO_SPEC_ATTACHMENT_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
        static constexpr VkFormat DEPTH_STENCIL_ATTACHMENT_FORMAT = VK_FORMAT_D32_SFLOAT_S8_UINT;

        VK_GBuffer(VK_Device& device, const glm::uvec2 size);
        ~VK_GBuffer();

        VK_GBuffer(const VK_GBuffer&) = delete;
        VK_GBuffer operator = (const VK_GBuffer&) = delete;

        void Recreate(const glm::uvec2 size);

        void CmdTransitionToRender(VkCommandBuffer commandBuffer);
        void CmdTransitionToSample(VkCommandBuffer commandBuffer);

        NODISCARD VkRenderingInfo* GetRenderingInfo() { return &m_RenderingInfo; }
        NODISCARD VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

        void CmdBindSampleDescriptorSet(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout);
    private:
        VK_Device& m_Device;
        glm::uvec2 m_Size;

        std::unique_ptr<VK_Image> m_Position;
        std::unique_ptr<VK_Image> m_Normal;
        std::unique_ptr<VK_Image> m_AlbedoSpec;
        std::unique_ptr<VK_Image> m_Depth;

        VkSampler m_Sampler = VK_NULL_HANDLE;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        VkRenderingInfo m_RenderingInfo{};
        VkRenderingAttachmentInfo m_DepthAttachment{};
        std::array<VkRenderingAttachmentInfo, 3> m_ColorAttachments{};

        void SetupRenderingInfo();
        void UpdateDescriptorSet();

        void CreateSampler();
        void CreateDescriptorSetLayout();
    };
}
