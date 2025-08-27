#include "VK_GBuffer.hpp"
#include "VK_Device.hpp"
#include "VK_Image.hpp"
#include "VK_DescriptorPool.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GBuffer::VK_GBuffer(VK_Device& device, const glm::uvec2 size)
        : m_Device(device), m_Size(size)
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = 3;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, 1, 0);

        CreateSampler();
        CreateDescriptorSetLayout();

        m_DescriptorSet = m_DescriptorPool->Allocate(m_DescriptorSetLayout);

        Recreate(m_Size);
    }

    VK_GBuffer::~VK_GBuffer()
    {
        vkDestroySampler(m_Device.Get(), m_Sampler, nullptr);
        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
    }

    void VK_GBuffer::Recreate(const glm::uvec2 size)
    {
        m_Size = size;

        if (m_Position) m_Position.reset();
        if (m_Normal) m_Normal.reset();
        if (m_AlbedoSpec) m_AlbedoSpec.reset();
        if (m_Depth) m_Depth.reset();

        // Position attachment (location 0)
        m_Position = std::make_unique<VK_Image>(m_Device, m_Size,
            POSITION_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Normal attachment (location 1)
        m_Normal = std::make_unique<VK_Image>(m_Device, m_Size,
            NORMAL_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Albedo-spec attachment (location 2)
        m_AlbedoSpec = std::make_unique<VK_Image>(m_Device, m_Size,
            ALBEDO_SPEC_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);

        // Depth attachment
        m_Depth = std::make_unique<VK_Image>(m_Device, m_Size,
            DEPTH_STENCIL_ATTACHMENT_FORMAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1);

        m_Depth->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0);

        SetupRenderingInfo();
        UpdateDescriptorSet();
    }

    void VK_GBuffer::CreateSampler()
    {
        auto samplerInfo = MakeInfo<VkSamplerCreateInfo>();
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;

        VK_CHECK_RESULT(vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &m_Sampler), "Failed to create gbuffer sampler!");
    }

    void VK_GBuffer::CmdTransitionToRender(VkCommandBuffer commandBuffer)
    {
        VK_Image::CmdTransitionLayout(commandBuffer, m_Position->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_Normal->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_AlbedoSpec->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);
    }

    void VK_GBuffer::CmdTransitionToSample(VkCommandBuffer commandBuffer)
    {
        VK_Image::CmdTransitionLayout(commandBuffer, m_Position->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_Normal->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

        VK_Image::CmdTransitionLayout(commandBuffer, m_AlbedoSpec->Get(), VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);
    }

    void VK_GBuffer::CmdBindSampleDescriptorSet(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout)
    {
        vkCmdBindDescriptorSets(
            cmdBuff,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0, 1,
            &m_DescriptorSet,
            0, nullptr
        );
    }

    void VK_GBuffer::SetupRenderingInfo()
    {
        // Position attachment
        m_ColorAttachments[0] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[0].imageView = m_Position->GetView();
        m_ColorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[0].clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

        // Normal attachment
        m_ColorAttachments[1] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[1].imageView = m_Normal->GetView();
        m_ColorAttachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[1].clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

        // Albedo-spec attachment
        m_ColorAttachments[2] = MakeInfo<VkRenderingAttachmentInfo>();
        m_ColorAttachments[2].imageView = m_AlbedoSpec->GetView();
        m_ColorAttachments[2].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_ColorAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_ColorAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_ColorAttachments[2].clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

        // Depth attachment
        m_DepthAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        m_DepthAttachment.imageView = m_Depth->GetView();
        m_DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        m_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_DepthAttachment.clearValue.depthStencil = {1.0f, 0};

        m_RenderingInfo = MakeInfo<VkRenderingInfo>();
        m_RenderingInfo.renderArea.offset = {0, 0};
        m_RenderingInfo.renderArea.extent = { m_Size.x, m_Size.y};
        m_RenderingInfo.layerCount = 1;
        m_RenderingInfo.colorAttachmentCount = m_ColorAttachments.size();
        m_RenderingInfo.pColorAttachments = m_ColorAttachments.data();
        m_RenderingInfo.pDepthAttachment = &m_DepthAttachment;
    }

    void VK_GBuffer::UpdateDescriptorSet()
    {
        std::array<VkDescriptorImageInfo, 3> imageInfos{};
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        imageInfos[0] = VkDescriptorImageInfo{
            .sampler = m_Sampler,
            .imageView = m_Position->GetView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        imageInfos[1] = VkDescriptorImageInfo{
            .sampler = m_Sampler,
            .imageView = m_Normal->GetView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        imageInfos[2] = VkDescriptorImageInfo{
            .sampler = m_Sampler,
            .imageView = m_AlbedoSpec->GetView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        // Position attachment (location 0)
        descriptorWrites[0] = MakeInfo<VkWriteDescriptorSet>();
        descriptorWrites[0].dstSet = m_DescriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfos[0];

        // Normal attachment (location 1)
        descriptorWrites[1] = MakeInfo<VkWriteDescriptorSet>();
        descriptorWrites[1].dstSet = m_DescriptorSet;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfos[1];

        // AlbedoSpec attachment (location 2)
        descriptorWrites[2] = MakeInfo<VkWriteDescriptorSet>();
        descriptorWrites[2].dstSet = m_DescriptorSet;
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &imageInfos[2];

        vkUpdateDescriptorSets(
            m_Device.Get(),
            descriptorWrites.size(),
            descriptorWrites.data(),
            0, nullptr
        );
    }

    void VK_GBuffer::CreateDescriptorSetLayout()
    {
        std::array<VkDescriptorSetLayoutBinding, 3> gBufferBindings{};

        for (uint32_t i = 0; i < gBufferBindings.size(); ++i)
        {
            gBufferBindings[i].binding = i;
            gBufferBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            gBufferBindings[i].descriptorCount = 1;
            gBufferBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        auto setLayoutInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        setLayoutInfo.bindingCount = static_cast<uint32_t>(gBufferBindings.size());
        setLayoutInfo.pBindings = gBufferBindings.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &setLayoutInfo, nullptr, &m_DescriptorSetLayout), "Failed to create descriptor set layout!");
    }
}
