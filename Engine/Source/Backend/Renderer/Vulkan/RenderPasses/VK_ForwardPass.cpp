#include "VK_ForwardPass.hpp"
#include "Assets/Shader.hpp"
#include "../Wrapper/VulkanWrapper.hpp"
#include "../Helpers/Vertex.hpp"
#include "../ImGui/VK_ImGUI_Renderer.hpp"
#include "Backend/Renderer/Vulkan/VK_BindlessManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Backend/Renderer/Vulkan/VK_GBuffer.hpp"
#include "Backend/Renderer/Vulkan/VK_GPUScene.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_ForwardPass::VK_ForwardPass(VK_Device& device, VK_Swapchain& swapchain, VK_GBuffer& gBuffer, VK_BindlessManager& bindlessManager, VK_GPUScene& gpuScene)
        : m_Device(device), m_Swapchain(swapchain), m_GBuffer(gBuffer), m_BindlessManager(bindlessManager), m_GPUScene(gpuScene)
    {
        Debug::Trace("Initializing forward pass.");

        for (uint32_t i = 0; i < m_Swapchain.GetFramesInFlightCount(); i++)
        {
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device, QueueType::Graphics));
        }

        CreateGraphicsPipeline();
    }

    VK_ForwardPass::~VK_ForwardPass()
    {
        Debug::Trace("Destroying forward pass.");
    }

    void VK_ForwardPass::CreateGraphicsPipeline()
    {
        auto shaderMetadata = ShaderMetadata();
        shaderMetadata.Paths[0] = "Assets/Engine/Shaders/ForwardPass.vert.spv";
        shaderMetadata.Paths[1] = "Assets/Engine/Shaders/ForwardPass.frag.spv";
        shaderMetadata.AddVariant("Main", 0, 1);

        auto shader = GetAssetManager()->Load<Shader>("ForwardPassShader", &shaderMetadata, true);

        if (!shader->IsOK())
        {
            Debug::Crash(ErrorCode::BUILT_IN_ASSET_NOT_LOADED,
                "Failed to load forward pass shader!", __FILE__, __LINE__);
        }

        const auto shaderVariant = shader->GetVariant("Main");

        auto configInfo = GraphicsPipelineConfigInfo::Make();

        configInfo.ShaderStages.resize(2);
        configInfo.ShaderStages[0] = shaderVariant.VertexModule->GetStageInfo();
        configInfo.ShaderStages[1] = shaderVariant.FragmentModule->GetStageInfo();

        configInfo.ColorAttachmentFormats = { m_Swapchain.GetImageFormat() };
        configInfo.DepthAttachmentFormat = VK_GBuffer::DEPTH_STENCIL_ATTACHMENT_FORMAT;

        configInfo.BlendAttachments.resize(1);
        configInfo.BlendAttachments[0].blendEnable = VK_TRUE;
        configInfo.BlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        configInfo.BlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        configInfo.BlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.BlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.BlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;
        configInfo.BlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
        configInfo.BlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        configInfo.Rasterizer.cullMode = VK_CULL_MODE_NONE;

        configInfo.DepthStencil.depthTestEnable = VK_TRUE;
        configInfo.DepthStencil.depthWriteEnable = VK_FALSE;

        configInfo.VertexBindingDescription = Vertex3p2t3n4g::GetBindingDescription();
        configInfo.VertexAttributeDescriptions = Vertex3p2t3n4g::GetAttributeDescriptions();

        const std::array descriptorSetLayouts = {
            m_BindlessManager.GetDescriptorSetLayout(),
            m_GPUScene.GetDescriptorSetLayout()
        };

        VkPushConstantRange pushConstants = {};
        pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstants.offset = 0;
        pushConstants.size = sizeof(uint32_t);

        auto pipelineLayoutCreateInfo = MakeInfo<VkPipelineLayoutCreateInfo>();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstants;
        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();

        const auto pipelineLayout = VK_GraphicsPipeline::CreateLayout(m_Device, pipelineLayoutCreateInfo);

        m_GraphicsPipeline = std::make_unique<VK_GraphicsPipeline>(m_Device, configInfo, pipelineLayout);
    }

    VkCommandBuffer VK_ForwardPass::RecordCommandBuffer(const AcquireImageInfo& swapchainImage, const uint32_t frameIndex)
    {
        const auto commandBuffer = m_CommandBuffers[frameIndex]->Get();

        m_CommandBuffers[frameIndex]->Reset(0);
        m_CommandBuffers[frameIndex]->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        auto colorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        colorAttachment.imageView = swapchainImage.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto depthAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        depthAttachment.imageView = m_GBuffer.GetDepth()->GetView();
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto renderingInfo = MakeInfo<VkRenderingInfo>();
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = m_Swapchain.GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        m_GraphicsPipeline->CmdBind(commandBuffer);
        m_GraphicsPipeline->CmdSetViewport(commandBuffer, glm::vec2(0.0f), m_Swapchain.GetSize());
        m_GraphicsPipeline->CmdSetScissor(commandBuffer, glm::ivec2(0), m_Swapchain.GetExtent());

        const std::array descriptorSets = {
            m_BindlessManager.GetDescriptorSet(),
            m_GPUScene.GetDescriptorSet(frameIndex)
        };

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_GraphicsPipeline->GetLayout(),
            0, descriptorSets.size(),
            descriptorSets.data(),
            0, nullptr
        );

        for (const auto& batch : m_GPUScene.GetForwardDrawBatches())
        {
            batch.VertexBuffer->CmdBind(commandBuffer);
            batch.IndexBuffer->CmdBind(commandBuffer);

            for (const auto& drawCall : batch.DrawCalls)
            {
                vkCmdPushConstants(
                    commandBuffer,
                    m_GraphicsPipeline->GetLayout(),
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(uint32_t),
                    &drawCall.MeshIndex
                );

                vkCmdDrawIndexed(
                    commandBuffer,
                    drawCall.IndexCount,
                    1, drawCall.FirstIndex,
                    drawCall.VertexOffset, 0
                );
            }
        }

        vkCmdEndRendering(commandBuffer);

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
}
