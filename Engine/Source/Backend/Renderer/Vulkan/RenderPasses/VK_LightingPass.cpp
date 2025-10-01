#include "VK_LightingPass.hpp"
#include "../Wrapper/VulkanWrapper.hpp"
#include "../Helpers/VulkanUtility.hpp"
#include "../../ShaderStructs.hpp"
#include "../ImGui/VK_ImGUI_Renderer.hpp"
#include "../VK_BindlessManager.hpp"
#include "../VK_GBuffer.hpp"
#include "Assets/Model.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/Renderer/SceneRenderInfo.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_LightingPass::VK_LightingPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer)
        : m_Device(device), m_Swapchain(swapchain), m_GBuffer(gBuffer)
    {
        Debug::Trace("Initializing lighting pass.");

        for (uint32_t i = 0; i < m_Swapchain.GetFramesInFlightCount(); ++i)
        {
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device, QueueType::Graphics));
        }

        CreateSampler();
        CreateDescriptorSet();
        CreateGraphicsPipeline();

        Recreate();
    }

    VK_LightingPass::~VK_LightingPass()
    {
        Debug::Trace("Destroying lighting pass.");

        vkDestroySampler(m_Device.Get(), m_GBufferSampler, nullptr);
        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
    }

    void VK_LightingPass::Recreate()
    {
        std::array<VkDescriptorImageInfo, 3> imageInfos{};
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        imageInfos[0] = VkDescriptorImageInfo{
            .sampler = m_GBufferSampler,
            .imageView = m_GBuffer.GetPosition()->GetView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        imageInfos[1] = VkDescriptorImageInfo{
            .sampler = m_GBufferSampler,
            .imageView = m_GBuffer.GetNormalRoughness()->GetView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        imageInfos[2] = VkDescriptorImageInfo{
            .sampler = m_GBufferSampler,
            .imageView = m_GBuffer.GetAlbedoMetallic()->GetView(),
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

    void VK_LightingPass::SetLightData(const Ref<SceneRenderInfo> sceneRenderInfo, const uint32_t frameIndex)
    {
        m_CamPos = sceneRenderInfo->CamPos;
    }

    VkCommandBuffer VK_LightingPass::RecordCommandBuffer(const AcquireImageInfo& swapchainImage, const uint32_t frameIndex)
    {
        const auto commandBuffer = m_CommandBuffers[frameIndex]->Get();

        m_CommandBuffers[frameIndex]->Reset(0);
        m_CommandBuffers[frameIndex]->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_Image::CmdTransitionLayout(commandBuffer, swapchainImage.image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);
        m_GBuffer.CmdTransitionToSample(commandBuffer);

        auto colorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        colorAttachment.imageView = swapchainImage.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};

        auto renderingInfo = MakeInfo<VkRenderingInfo>();
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = m_Swapchain.GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        m_GraphicsPipeline->CmdBind(commandBuffer);
        m_GraphicsPipeline->CmdSetViewport(commandBuffer, glm::vec2(0.0f), m_Swapchain.GetSize());
        m_GraphicsPipeline->CmdSetScissor(commandBuffer, glm::ivec2(0), m_Swapchain.GetExtent());

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_GraphicsPipeline->GetLayout(),
            0, 1,
            &m_DescriptorSet,
            0, nullptr
        );

        vkCmdPushConstants(
            commandBuffer,
            m_GraphicsPipeline->GetLayout(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(glm::vec3),
            &m_CamPos
        );

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRendering(commandBuffer);

        // Until forward pass is implemented UI drawing stays here
        auto uiColorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        uiColorAttachment.imageView = swapchainImage.imageView;
        uiColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        uiColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        uiColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto uiRenderingInfo = MakeInfo<VkRenderingInfo>();
        uiRenderingInfo.renderArea.offset = {0, 0};
        uiRenderingInfo.renderArea.extent = m_Swapchain.GetExtent();
        uiRenderingInfo.layerCount = 1;
        uiRenderingInfo.colorAttachmentCount = 1;
        uiRenderingInfo.pColorAttachments = &uiColorAttachment;

        vkCmdBeginRendering(commandBuffer, &uiRenderingInfo);
        m_ImGuiBackend->RenderFrame(commandBuffer);
        vkCmdEndRendering(commandBuffer);

        VK_Image::CmdTransitionLayout(commandBuffer, swapchainImage.image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0);

        m_CommandBuffers[frameIndex]->EndRecording();
        return commandBuffer;
    }

    void VK_LightingPass::CreateSampler()
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

        VK_CHECK_RESULT(vkCreateSampler(m_Device.Get(), &samplerInfo, nullptr, &m_GBufferSampler), "Failed to create gbuffer sampler!");
    }

    void VK_LightingPass::CreateDescriptorSet()
    {
        std::array<VkDescriptorSetLayoutBinding, 3> bindings{};

        for (uint32_t i = 0; i < bindings.size(); ++i)
        {
            bindings[i].binding = i;
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindings[i].descriptorCount = 1;
            bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        auto setLayoutInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        setLayoutInfo.bindingCount = bindings.size();
        setLayoutInfo.pBindings = bindings.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &setLayoutInfo, nullptr, &m_DescriptorSetLayout), "Failed to create descriptor set layout!");

        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = 3;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, 1, 0);
        m_DescriptorSet = m_DescriptorPool->Allocate(m_DescriptorSetLayout);
    }

    void VK_LightingPass::CreateGraphicsPipeline()
    {
        VkPushConstantRange pushConstants = {};
        pushConstants.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstants.offset = 0;
        pushConstants.size = sizeof(glm::vec3);

        auto pipelineLayout = MakeInfo<VkPipelineLayoutCreateInfo>();
        pipelineLayout.pushConstantRangeCount = 1;
        pipelineLayout.pPushConstantRanges = &pushConstants;
        pipelineLayout.setLayoutCount = 0;
        pipelineLayout.setLayoutCount = 1;
        pipelineLayout.pSetLayouts = &m_DescriptorSetLayout;

        auto lightingPassShaderMetadata = ShaderMetadata();
        lightingPassShaderMetadata.Paths[0] = "Assets/Engine/Shaders/DirLight.vert.spv";
        lightingPassShaderMetadata.Paths[1] = "Assets/Engine/Shaders/DirLight.frag.spv";
        lightingPassShaderMetadata.AddVariant("Main", 0, 1);

        auto lightingPassShader = GetAssetManager()->Load<Shader>("LightingPassShader", &lightingPassShaderMetadata, true);

        m_GraphicsPipeline = VK_GraphicsPipeline::CreateLightingPassPipeline(
            m_Device,
            pipelineLayout,
            m_Swapchain.GetImageFormat(),
            lightingPassShader->GetVariant("Main")
        );
    }
}
