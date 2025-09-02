#include "VK_GeometryPass.hpp"
#include "../VK_BindlessManager.hpp"
#include "../VK_GBuffer.hpp"
#include "../Wrapper/VulkanWrapper.hpp"
#include "../Helpers/VulkanUtility.hpp"
#include "../../ShaderStructs.hpp"
#include "Assets/Model.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/Renderer/SceneRenderInfo.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GeometryPass::VK_GeometryPass(VK_Device& device, VK_Swapchain& swapchain, VK_BindlessManager& bindlessManager, VK_GBuffer& gBuffer)
        : m_Device(device), m_Swapchain(swapchain), m_BindlessManager(bindlessManager), m_GBuffer(gBuffer)
    {
        const auto framesInFlight = m_Swapchain.GetFramesInFlightCount();

        for (uint32_t i = 0; i < framesInFlight; ++i)
        {
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(m_Device, QueueType::Graphics));
            m_MeshBuffers.emplace_back(std::make_unique<VK_MemoryBuffer>(m_Device, sizeof(MeshData) * MAX_MESHES,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU));
        }

        m_Meshes.resize(MAX_MESHES);

        CreateDescriptorSet();
        CreateGraphicsPipeline();
    }

    VK_GeometryPass::~VK_GeometryPass()
    {
        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
    }

    void VK_GeometryPass::CreateDescriptorSet()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings(1);

        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        auto setLayoutInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        setLayoutInfo.bindingCount = bindings.size();
        setLayoutInfo.pBindings = bindings.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &setLayoutInfo, nullptr, &m_DescriptorSetLayout), "Failed to create descriptor set layout!");

        const auto framesInFlight = m_Swapchain.GetFramesInFlightCount();

        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[0].descriptorCount = framesInFlight;

        m_DescriptorPool = std::make_unique<VK_DescriptorPool>(m_Device, poolSizes, framesInFlight, 0);
        for (uint32_t i = 0; i < framesInFlight; ++i)
        {
            const auto set = m_DescriptorPool->Allocate(m_DescriptorSetLayout);
            m_DescriptorSets.push_back(set);

            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = m_MeshBuffers[i]->Get();
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            auto write = MakeInfo<VkWriteDescriptorSet>();
            write.dstSet = set;
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(m_Device.Get(), 1, &write, 0, nullptr);
        }
    }

    void VK_GeometryPass::CreateGraphicsPipeline()
    {
        const std::vector descriptorSetLayouts = {
            m_BindlessManager.GetDescriptorSetLayout(),
            m_DescriptorSetLayout
        };

        VkPushConstantRange pushConstants = {};
        pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstants.offset = 0;
        pushConstants.size = sizeof(uint32_t);

        auto pipelineLayout = MakeInfo<VkPipelineLayoutCreateInfo>();
        pipelineLayout.pushConstantRangeCount = 1;
        pipelineLayout.pPushConstantRanges = &pushConstants;
        pipelineLayout.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayout.pSetLayouts = descriptorSetLayouts.data();

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

        m_GraphicsPipeline = VK_GraphicsPipeline::CreateGeometryPassPipeline(
            m_Device,
            pipelineLayout,
            shader->GetVariant("Main")
        );
    }

    void VK_GeometryPass::SetMeshData(const Ref<SceneRenderInfo> sceneRenderInfo, const uint32_t frameIndex)
    {
        m_DrawBatches.clear();

        if (!sceneRenderInfo->CameraPresent)
            return;

        uint32_t meshIndex = 0;
        const auto projView = sceneRenderInfo->ProjView;

        for (uint32_t i = 0; i < sceneRenderInfo->Models.size(); ++i)
        {
            const auto& modelAsset = sceneRenderInfo->Models[i];
            const auto& modelTransform = sceneRenderInfo->Transforms[i];

            auto batch = BakedDrawBatch();
            batch.VertexBuffer = modelAsset->GetVertexBuffer();
            batch.IndexBuffer = modelAsset->GetIndexBuffer();

            int32_t vertexOffset = 0;
            for (auto node = modelAsset->GetNodeIterator(); node.Valid(); ++node)
            {
                const auto modelMat = modelTransform * node->Transform;
                const auto normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
                const auto MVP = projView * modelMat;

                for (const auto& mesh : node->Meshes)
                {
                    m_Meshes[meshIndex] = MeshData{
                        .MaterialIndex = mesh.Material->GetBindlessIndex(),
                        .MVP = MVP,
                        .Model = modelMat,
                        .Normal = normalMat,
                    };

                    batch.DrawCalls.push_back({
                        .MeshIndex = meshIndex,
                        .IndexCount = mesh.IndexCount,
                        .FirstIndex = mesh.FirstIndex,
                        .VertexOffset = vertexOffset
                    });

                    vertexOffset = static_cast<int32_t>(mesh.FirstVertex) + static_cast<int32_t>(mesh.VertexCount);
                    ++meshIndex;
                }
            }

            m_DrawBatches.push_back(batch);
        }

        // Update current frame's mesh buffer
        const auto& buffer = m_MeshBuffers[frameIndex];
        buffer->UploadData(0, sizeof(MeshData) * m_Meshes.size(), m_Meshes.data());
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
            m_DescriptorSets[frameIndex]
        };

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_GraphicsPipeline->GetLayout(),
            0, descriptorSets.size(),
            descriptorSets.data(),
            0, nullptr
        );

        for (const auto& batch : m_DrawBatches)
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
