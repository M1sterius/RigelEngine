#include "VK_GeometryPass.hpp"
#include "../VK_BindlessManager.hpp"
#include "../VK_GBuffer.hpp"
#include "../VK_GPUScene.hpp"
#include "../Wrapper/VulkanWrapper.hpp"
#include "../Helpers/VulkanUtility.hpp"
#include "../Helpers/Vertex.hpp"
#include "../Wrapper/VK_ShaderModule.hpp"
#include "../../ShaderStructs.hpp"
#include "Assets/Model.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/Renderer/SceneRenderInfo.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GeometryPass::VK_GeometryPass(VK_Device& device, VK_Swapchain& swapchain, VK_BindlessManager& bindlessManager, VK_GBuffer& gBuffer, VK_GPUScene& gpuScene)
        : m_Device(device), m_Swapchain(swapchain), m_BindlessManager(bindlessManager), m_GBuffer(gBuffer), m_GPUScene(gpuScene)
    {
        Debug::Trace("Initializing geometry pass.");

        for (uint32_t i = 0; i < m_Swapchain.GetFramesInFlightCount(); ++i)
        {
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device, QueueType::Graphics));
        }

        CreateGraphicsPipeline();
    }

    VK_GeometryPass::~VK_GeometryPass()
    {
        Debug::Trace("Destroying geometry pass.");
    }

    void VK_GeometryPass::CreateGraphicsPipeline()
    {
        auto shaderMetadata = ShaderMetadata();
        shaderMetadata.Paths[0] = "Assets/Engine/Shaders/GeometryPass.vert.spv";
        shaderMetadata.Paths[1] = "Assets/Engine/Shaders/GeometryPass.frag.spv";
        shaderMetadata.AddVariant("Main", 0, 1);

        auto shader = GetAssetManager()->Load<Shader>("GeometryPassShader", &shaderMetadata, true);

        if (!shader->IsOK())
        {
            Debug::Crash(ErrorCode::BUILT_IN_ASSET_NOT_LOADED,
                "Failed to load geometry pass shader!", __FILE__, __LINE__);
        }

        const auto shaderVariant = shader->GetVariant("Main");

        auto configInfo = GraphicsPipelineConfigInfo::Make();

        configInfo.ShaderStages.resize(2);
        configInfo.ShaderStages[0] = shaderVariant.VertexModule->GetStageInfo();
        configInfo.ShaderStages[1] = shaderVariant.FragmentModule->GetStageInfo();

        configInfo.ColorAttachmentFormats = {
            VK_GBuffer::POSITION_ATTACHMENT_FORMAT,
            VK_GBuffer::NORMAL_ROUGHNESS_ATTACHMENT_FORMAT,
            VK_GBuffer::ALBEDO_METALLIC_ATTACHMENT_FORMAT
        };

        configInfo.DepthAttachmentFormat = VK_GBuffer::DEPTH_STENCIL_ATTACHMENT_FORMAT;
        configInfo.StencilAttachmentFormat = VK_GBuffer::DEPTH_STENCIL_ATTACHMENT_FORMAT;

        configInfo.VertexBindingDescription = Vertex3p2t3n4g::GetBindingDescription();
        configInfo.VertexAttributeDescriptions = Vertex3p2t3n4g::GetAttributeDescriptions();

        configInfo.BlendAttachments.resize(3);
        configInfo.BlendAttachments[0].blendEnable = VK_FALSE;
        configInfo.BlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        configInfo.BlendAttachments[1].blendEnable = VK_FALSE;
        configInfo.BlendAttachments[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        configInfo.BlendAttachments[2].blendEnable = VK_FALSE;
        configInfo.BlendAttachments[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

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

    VkCommandBuffer VK_GeometryPass::RecordCommandBuffer(const uint32_t frameIndex)
    {
        const auto commandBuffer = m_CommandBuffers[frameIndex]->Get();

        m_CommandBuffers[frameIndex]->Reset(0);
        m_CommandBuffers[frameIndex]->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        m_GBuffer.CmdTransitionToRender(commandBuffer);

        vkCmdBeginRendering(commandBuffer, m_GBuffer.GetRenderingInfo());

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

        for (const auto& batch : m_GPUScene.GetDeferredDrawBatches())
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

        m_CommandBuffers[frameIndex]->EndRecording();
        return commandBuffer;
    }
}
