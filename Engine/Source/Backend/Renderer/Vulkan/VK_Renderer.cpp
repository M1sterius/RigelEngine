#include "VK_Renderer.hpp"
#include "Assets/Shader.hpp"
#include "Assets/Model.hpp"
#include "ShaderStructs.hpp"
#include "VK_BindlessManager.hpp"
#include "VK_GBuffer.hpp"
#include "VulkanWrapper.hpp"
#include "VulkanUtility.hpp"
#include "MakeInfo.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"
#include "Subsystems/Time.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/AssetManager/BuiltInAssets.hpp"
#include "Subsystems/WindowManager/WindowManager.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/SubsystemGetters.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    static constexpr auto GEOMETRY_PASS_PIPELINE_NAME = "GeometryPass";

    VK_Renderer::VK_Renderer() = default;
    VK_Renderer::~VK_Renderer() = default;

    ErrorCode VK_Renderer::Startup()
    {
        Debug::Trace("Starting up Vulkan renderer backend.");

        m_Instance = std::make_unique<VK_Instance>();
        m_Surface = std::make_unique<VK_Surface>(m_Instance->Get());
        m_Device = std::make_unique<VK_Device>(m_Instance->Get(), m_Surface->Get());
        m_Swapchain = std::make_unique<VK_Swapchain>(*m_Device, m_Surface->Get(), GetWindowManager()->GetWindowSize());
        m_BindlessManager = std::make_unique<VK_BindlessManager>(*this, *m_Device);
        m_GBuffer = std::make_unique<VK_GBuffer>(*m_Device, GetWindowManager()->GetWindowSize());

        const auto framesInFlight = m_Swapchain->GetFramesInFlightCount();

        for (uint32_t i = 0; i < framesInFlight; ++i)
        {
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));
            m_RenderFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
            m_LightingPassCommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device, QueueType::Graphics));

            m_GeometryPassCommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device, QueueType::Graphics));
            m_GeometryPassFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
        }

        CreateStagingBuffers();

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::LateStartup()
    {
        Debug::Trace("Vulkan renderer late startup.");
        ASSERT(m_ImGuiBackend, "ImGui backend was a nullptr");

        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureError, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureBlack, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureWhite, true);

        if (const auto result = SetupPipelines(); result != ErrorCode::OK)
            return result;

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::Shutdown()
    {
        // this shutdown method is called inside Renderer::Shutdown()!
        m_Device->WaitIdle();

        m_StagingBuffers.clear();

        Debug::Trace("Shutting down Vulkan renderer.");

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::SetupPipelines()
    {
        // Geometry pass
        const std::vector descriptorSetLayouts = {m_BindlessManager->GetDescriptorSetLayout()};

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        auto geometryPassPipelineLayout = MakeInfo<VkPipelineLayoutCreateInfo>();
        geometryPassPipelineLayout.pushConstantRangeCount = 1;
        geometryPassPipelineLayout.pPushConstantRanges = &pushConstantRange;
        geometryPassPipelineLayout.setLayoutCount = descriptorSetLayouts.size();
        geometryPassPipelineLayout.pSetLayouts = descriptorSetLayouts.data();

        auto geometryPassShaderMetadata = ShaderMetadata();
        geometryPassShaderMetadata.Paths[0] = "Assets/Engine/Shaders/GeometryPass.vert.spv";
        geometryPassShaderMetadata.Paths[1] = "Assets/Engine/Shaders/GeometryPass.frag.spv";
        geometryPassShaderMetadata.AddVariant("Main", 0, 1);

        auto geometryPassShader = GetAssetManager()->Load<Shader>("GeometryPassShader", &geometryPassShaderMetadata, true);

        m_GeometryPassPipeline = VK_GraphicsPipeline::CreateGeometryPassPipeline(
            *m_Device,
            geometryPassPipelineLayout,
            geometryPassShader->GetVariant("Main")
        );
        // --------------

        // Lighting pass
        const auto gBufferSetLayout = m_GBuffer->GetDescriptorSetLayout();

        auto lightingPassPipelineLayout = MakeInfo<VkPipelineLayoutCreateInfo>();
        lightingPassPipelineLayout.pushConstantRangeCount = 0;
        lightingPassPipelineLayout.setLayoutCount = 0;
        lightingPassPipelineLayout.setLayoutCount = 1;
        lightingPassPipelineLayout.pSetLayouts = &gBufferSetLayout;

        auto lightingPassShaderMetadata = ShaderMetadata();
        lightingPassShaderMetadata.Paths[0] = "Assets/Engine/Shaders/DirLight.vert.spv";
        lightingPassShaderMetadata.Paths[1] = "Assets/Engine/Shaders/DirLight.frag.spv";
        lightingPassShaderMetadata.AddVariant("Main", 0, 1);

        auto lightingPassShader = GetAssetManager()->Load<Shader>("LightingPassShader", &lightingPassShaderMetadata, true);

        m_LightingPassPipeline = VK_GraphicsPipeline::CreateLightingPassPipeline(
            *m_Device,
            lightingPassPipelineLayout,
            m_Swapchain->GetImageFormat(),
            lightingPassShader->GetVariant("Main")
            );
        // --------------

        return ErrorCode::OK;
    }

    void VK_Renderer::OnRecreateSwapchain()
    {
        GetWindowManager()->WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = GetWindowManager()->GetWindowSize();
        const auto vsync = GetWindowManager()->IsVsyncEnabled();

        m_Swapchain->Setup(windowSize, vsync);
        m_GBuffer->SetupImages(windowSize);
    }

    VK_MemoryBuffer& VK_Renderer::GetStagingBuffer() const
    {
        const auto thisThreadID = std::this_thread::get_id();
        if (!m_StagingBuffers.contains(thisThreadID))
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Staging buffer can only be retrieved for one of asset manager's loading threads or the main thread!", __FILE__, __LINE__);
        }

        return *m_StagingBuffers.at(thisThreadID);
    }

    void VK_Renderer::CreateStagingBuffers()
    {
        m_StagingBuffers[std::this_thread::get_id()] = std::make_unique<VK_MemoryBuffer>(*m_Device, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);

        for (const auto& id : GetAssetManager()->GetLoadingThreadsIDs())
        {
            m_StagingBuffers[id] = std::make_unique<VK_MemoryBuffer>(*m_Device, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
        }
    }

    void VK_Renderer::RecordGeometryPass(VkCommandBuffer cmdBuff)
    {
        m_GBuffer->CmdTransitionToRender(cmdBuff);

        vkCmdBeginRendering(cmdBuff, m_GBuffer->GetRenderingInfo());

        m_GeometryPassPipeline->CmdBind(cmdBuff);
        m_GeometryPassPipeline->CmdSetViewport(cmdBuff, glm::vec2(0.0f), m_Swapchain->GetSize());
        m_GeometryPassPipeline->CmdSetScissor(cmdBuff, glm::ivec2(0), m_Swapchain->GetExtent());

        m_BindlessManager->BindDescriptorSet(cmdBuff, m_GeometryPassPipeline->GetLayout());

        if (const auto sceneRenderInfo = GetRenderer()->GetSceneRenderInfo(); sceneRenderInfo->CameraPresent)
        {
            const auto sceneDataPtr = m_BindlessManager->GetSceneDataPtr();
            uint32_t meshIndex = 0;

            for (uint32_t i = 0; i < sceneRenderInfo->Models.size(); ++i)
            {
                const auto& model = sceneRenderInfo->Models[i];
                const auto& modelTransform = sceneRenderInfo->Transforms[i];

                model->GetVertexBuffer()->CmdBind(cmdBuff);
                model->GetIndexBuffer()->CmdBind(cmdBuff);

                int32_t vertexOffset = 0;
                for (auto node = model->GetNodeIterator(); node.Valid(); ++node)
                {
                    const auto meshModelMat = modelTransform * node->Transform;
                    const auto meshMVP = sceneRenderInfo->ProjView * meshModelMat;
                    const auto meshNormalMat = glm::transpose(glm::inverse(meshModelMat));

                    for (const auto& mesh : node->Meshes)
                    {
                        sceneDataPtr->Meshes[meshIndex] = {
                            .MaterialIndex = mesh.Material->GetBindlessIndex(),
                            .MVP = meshMVP,
                            .ModelMat = meshModelMat,
                            .NormalMat = meshNormalMat,
                        };

                        auto pc = PushConstantData{
                            .MeshIndex = meshIndex
                        };

                        vkCmdPushConstants(
                            cmdBuff,
                            m_GeometryPassPipeline->GetLayout(),
                            VK_SHADER_STAGE_VERTEX_BIT,
                            0,
                            sizeof(pc),
                            &pc
                        );

                        vkCmdDrawIndexed(
                            cmdBuff,
                            mesh.IndexCount,
                            1,
                            mesh.FirstIndex,
                            vertexOffset,
                            0
                        );

                        vertexOffset = mesh.FirstVertex + mesh.VertexCount;
                        ++meshIndex;
                    }
                }
            }
        }

        vkCmdEndRendering(cmdBuff);
    }

    void VK_Renderer::RecordLightingPass(VkCommandBuffer cmdBuff, const AcquireImageInfo& swapchainImage)
    {
        // TODO: This is just a test pass to get something rendering on the screen while I test if geometry pass is working as expected

        VK_Image::CmdTransitionLayout(cmdBuff, swapchainImage.image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        m_GBuffer->CmdTransitionToSample(cmdBuff);

        auto colorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        colorAttachment.imageView = swapchainImage.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};

        auto renderingInfo = MakeInfo<VkRenderingInfo>();
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = m_Swapchain->GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(cmdBuff, &renderingInfo);

        m_LightingPassPipeline->CmdBind(cmdBuff);
        m_LightingPassPipeline->CmdSetViewport(cmdBuff, glm::vec2(0.0f), m_Swapchain->GetSize());
        m_LightingPassPipeline->CmdSetScissor(cmdBuff, glm::ivec2(0), m_Swapchain->GetExtent());

        const auto& descriptorWrites = m_GBuffer->GetDescriptorWrites();

        vkCmdPushDescriptorSet(
            cmdBuff,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_LightingPassPipeline->GetLayout(),
            0,
            descriptorWrites.size(),
            descriptorWrites.data()
        );

        vkCmdDraw(cmdBuff, 3, 1, 0, 0);

        vkCmdEndRendering(cmdBuff);

        auto uiColorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        uiColorAttachment.imageView = swapchainImage.imageView;
        uiColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        uiColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        uiColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto uiRenderingInfo = MakeInfo<VkRenderingInfo>();
        uiRenderingInfo.renderArea.offset = {0, 0};
        uiRenderingInfo.renderArea.extent = m_Swapchain->GetExtent();
        uiRenderingInfo.layerCount = 1;
        uiRenderingInfo.colorAttachmentCount = 1;
        uiRenderingInfo.pColorAttachments = &uiColorAttachment;

        vkCmdBeginRendering(cmdBuff, &uiRenderingInfo);
        m_ImGuiBackend->RenderFrame(cmdBuff);
        vkCmdEndRendering(cmdBuff);

        VK_Image::CmdTransitionLayout(cmdBuff, swapchainImage.image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0);
    }

    void VK_Renderer::Render()
    {
        if (GetWindowManager()->GetWindowResizeFlag())
        {
            GetWindowManager()->ResetWindowResizeFlag();
            OnRecreateSwapchain();
            return;
        }

        const auto frameIndex = Time::GetFrameCount() % m_Swapchain->GetFramesInFlightCount();
        m_InFlightFences[frameIndex]->Wait();

        const auto swapchainImage = m_Swapchain->AcquireNextImage();

        const auto& geometryPassCommandBuffer = m_GeometryPassCommandBuffers[frameIndex];
        const auto& lightingPassCommandBuffer = m_LightingPassCommandBuffers[frameIndex];

        geometryPassCommandBuffer->Reset(0);
        lightingPassCommandBuffer->Reset(0);

        geometryPassCommandBuffer->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        RecordGeometryPass(geometryPassCommandBuffer->Get());
        geometryPassCommandBuffer->EndRecording();

        lightingPassCommandBuffer->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        RecordLightingPass(lightingPassCommandBuffer->Get(), swapchainImage);
        lightingPassCommandBuffer->EndRecording();

        m_BindlessManager->UpdateStorageBuffer(frameIndex); // this line must stay after geometry pass recording

        const auto& fence = m_InFlightFences[frameIndex];
        fence->Reset();

        const VkCommandBuffer geometryPassSubmitBuffers[] = {geometryPassCommandBuffer->Get()};
        const VkSemaphore geometryPassSignalSemaphores[] = {m_GeometryPassFinishedSemaphores[frameIndex]->Get()};

        auto geometryPassSubmitInfo = MakeInfo<VkSubmitInfo>();
        geometryPassSubmitInfo.pWaitDstStageMask = nullptr;
        geometryPassSubmitInfo.commandBufferCount = 1;
        geometryPassSubmitInfo.pCommandBuffers = geometryPassSubmitBuffers;
        geometryPassSubmitInfo.waitSemaphoreCount = 0;
        geometryPassSubmitInfo.signalSemaphoreCount = 1;
        geometryPassSubmitInfo.pSignalSemaphores = geometryPassSignalSemaphores;

        m_Device->SubmitToQueue(QueueType::Graphics, 1, &geometryPassSubmitInfo, nullptr);

        const VkCommandBuffer lightingPassSubmitBuffers[] = {lightingPassCommandBuffer->Get()};
        const VkSemaphore lightingPassWaitSemaphores[] = {
            m_GeometryPassFinishedSemaphores[frameIndex]->Get(),
            swapchainImage.availableSemaphore
        };
        const VkSemaphore lightingPassSignalSemaphores[] = {m_RenderFinishedSemaphores[frameIndex]->Get()};

        constexpr VkPipelineStageFlags lightingPassWaitStages[] = {
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        auto lightingPassSubmitInfo = MakeInfo<VkSubmitInfo>();

        lightingPassSubmitInfo.pWaitDstStageMask = lightingPassWaitStages;
        lightingPassSubmitInfo.commandBufferCount = 1;
        lightingPassSubmitInfo.pCommandBuffers = lightingPassSubmitBuffers;
        lightingPassSubmitInfo.waitSemaphoreCount = 2;
        lightingPassSubmitInfo.pWaitSemaphores = lightingPassWaitSemaphores;
        lightingPassSubmitInfo.signalSemaphoreCount = 1;
        lightingPassSubmitInfo.pSignalSemaphores = lightingPassSignalSemaphores;

        m_Device->SubmitToQueue(QueueType::Graphics, 1, &lightingPassSubmitInfo, fence->Get());

        VkSemaphore presentWaitSemaphore = m_RenderFinishedSemaphores[frameIndex]->Get();

        m_Swapchain->Present(swapchainImage.imageIndex, presentWaitSemaphore);
    }

    void VK_Renderer::WaitForFinish() const
    {
        m_Device->WaitIdle();
    }
}
