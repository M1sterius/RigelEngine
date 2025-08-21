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

        CreateStagingBuffers();
        CreateDepthBufferImage(GetWindowManager()->GetWindowSize());

        const auto framesInFlight = m_Swapchain->GetFramesInFlightCount();

        for (uint32_t i = 0; i < framesInFlight; ++i)
        {
            m_InFlightFences.emplace_back(std::make_unique<VK_Fence>(*m_Device, true));
            m_RenderFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
            m_CommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device, QueueType::Graphics));

            m_GeometryPassCommandBuffers.emplace_back(std::make_unique<VK_CmdBuffer>(*m_Device, QueueType::Graphics));
            m_GeometryPassFinishedSemaphores.emplace_back(std::make_unique<VK_Semaphore>(*m_Device));
        }

        return ErrorCode::OK;
    }

    ErrorCode VK_Renderer::LateStartup()
    {
        Debug::Trace("Vulkan renderer late startup.");
        ASSERT(m_ImGuiBackend, "ImGui backend was a nullptr");

        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureError, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureBlack, true);
        GetAssetManager()->Load<Texture2D>(BuiltInAssets::TextureWhite, true);

        auto defaultShaderMetadata = ShaderMetadata();
        defaultShaderMetadata.Paths[0] = "Assets/Engine/Shaders/DefaultShader.vert.spv";
        defaultShaderMetadata.Paths[1] = "Assets/Engine/Shaders/DefaultShader.frag.spv";
        defaultShaderMetadata.AddVariant("Main", 0, 1);

        auto defaultShader = GetAssetManager()->Load<Shader>(BuiltInAssets::ShaderDefault, &defaultShaderMetadata, true);
        if (!defaultShader->IsOK())
            return ErrorCode::BUILT_IN_ASSET_NOT_LOADED;

        const std::vector layouts = { m_BindlessManager->GetDescriptorSetLayout() };

        m_GraphicsPipeline = VK_GraphicsPipeline::CreateDefaultGraphicsPipeline(
            *m_Device,
            m_Swapchain->GetSwapchainImageFormat(),
            defaultShader->GetVariant("Main"),
            layouts
        );

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

    void VK_Renderer::OnRecreateSwapchain()
    {
        GetWindowManager()->WaitForFocus();
        m_Device->WaitIdle();

        const auto windowSize = GetWindowManager()->GetWindowSize();
        const auto vsync = GetWindowManager()->IsVsyncEnabled();

        CreateDepthBufferImage(windowSize);
        m_Swapchain->Setup(windowSize, vsync);
        m_GBuffer->Setup(windowSize);
    }

    void VK_Renderer::CreateDepthBufferImage(const glm::uvec2 size)
    {
        if (m_DepthBufferImage)
            m_DepthBufferImage.reset();

        m_DepthBufferImage = std::make_unique<VK_Image>(*m_Device, size, VK_FORMAT_D32_SFLOAT,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        m_DepthBufferImage->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, 0);
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

    void VK_Renderer::RenderScene(VkCommandBuffer vkCmdBuffer)
    {
        const auto sceneRenderInfo = GetRenderer()->GetSceneRenderInfo();

        if (!sceneRenderInfo->CameraPresent)
            return;

        const auto sceneDataPtr = m_BindlessManager->GetSceneDataPtr();
        uint32_t meshIndex = 0;

        for (uint32_t i = 0; i < sceneRenderInfo->Models.size(); ++i)
        {
            const auto& model = sceneRenderInfo->Models[i];
            const auto& modelTransform = sceneRenderInfo->Transforms[i];

            model->GetVertexBuffer()->CmdBind(vkCmdBuffer);
            model->GetIndexBuffer()->CmdBind(vkCmdBuffer);

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
                        vkCmdBuffer,
                        m_GraphicsPipeline->GetLayout(),
                        VK_SHADER_STAGE_VERTEX_BIT,
                        0,
                        sizeof(pc),
                        &pc
                    );

                    vkCmdDrawIndexed(
                        vkCmdBuffer,
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

    void VK_Renderer::RecordGeometryPass(VkCommandBuffer cmdBuff)
    {
        m_GBuffer->CmdTransitionToRender(cmdBuff);

        const auto renderingInfo = m_GBuffer->GetRenderingInfo();
        vkCmdBeginRendering(cmdBuff, renderingInfo);

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
                            m_GraphicsPipeline->GetLayout(),
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

    void VK_Renderer::RecordCommandBuffer(const std::unique_ptr<VK_CmdBuffer>& commandBuffer, const AcquireImageInfo& image)
    {
        const auto vkCmdBuffer = commandBuffer->Get();

        VK_Image::CmdTransitionLayout(vkCmdBuffer, image.image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

        auto colorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        colorAttachment.imageView = image.imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.2f, 0.3f, 0.4f, 1.0f}};

        auto depthAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        depthAttachment.imageView = m_DepthBufferImage->GetView();
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};

        auto renderingInfo = MakeInfo<VkRenderingInfo>();
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = m_Swapchain->GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;

        vkCmdBeginRendering(vkCmdBuffer, &renderingInfo);

        m_GraphicsPipeline->CmdBind(vkCmdBuffer);
        m_GraphicsPipeline->CmdSetViewport(vkCmdBuffer, glm::vec2(0.0f), m_Swapchain->GetSize());
        m_GraphicsPipeline->CmdSetScissor(vkCmdBuffer, glm::ivec2(0), m_Swapchain->GetExtent());
        m_GraphicsPipeline->CmdSetDepthTestEnable(vkCmdBuffer, true);
        m_GraphicsPipeline->CmdSetDepthWriteEnable(vkCmdBuffer, true);
        m_GraphicsPipeline->CmdSetCullMode(vkCmdBuffer, VK_CULL_MODE_BACK_BIT);

        m_BindlessManager->BindDescriptorSet(vkCmdBuffer, m_GraphicsPipeline->GetLayout());

        RenderScene(vkCmdBuffer);

        vkCmdEndRendering(vkCmdBuffer);

        auto uiColorAttachment = MakeInfo<VkRenderingAttachmentInfo>();
        uiColorAttachment.imageView = image.imageView;
        uiColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        uiColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        uiColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto uiRenderingInfo = MakeInfo<VkRenderingInfo>();
        uiRenderingInfo.renderArea = { {0, 0}, m_Swapchain->GetExtent() };
        uiRenderingInfo.layerCount = 1;
        uiRenderingInfo.colorAttachmentCount = 1;
        uiRenderingInfo.pColorAttachments = &uiColorAttachment;

        vkCmdBeginRendering(vkCmdBuffer, &uiRenderingInfo);
        m_ImGuiBackend->RenderFrame(commandBuffer->Get());
        vkCmdEndRendering(vkCmdBuffer);

        VK_Image::CmdTransitionLayout(vkCmdBuffer, image.image, VK_IMAGE_ASPECT_COLOR_BIT,
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

        const auto image = m_Swapchain->AcquireNextImage();

        const auto& commandBuffer = m_CommandBuffers[frameIndex];
        commandBuffer->Reset(0);

        commandBuffer->BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        RecordCommandBuffer(commandBuffer, image);
        commandBuffer->EndRecording();

        m_BindlessManager->UpdateStorageBuffer(frameIndex);

        const VkCommandBuffer submitBuffers[] = { commandBuffer->Get() };
        const VkSemaphore waitSemaphores[] = { image.availableSemaphore };
        const VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[frameIndex]->Get() };

        constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        auto submitInfo = MakeInfo<VkSubmitInfo>();
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = submitBuffers;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        const auto& fence = m_InFlightFences[frameIndex];
        fence->Reset();

        m_Device->SubmitToQueue(QueueType::Graphics,1, &submitInfo, fence->Get());

        m_Swapchain->Present(image.imageIndex, signalSemaphores[0]);
    }

    void VK_Renderer::WaitForFinish() const
    {
        m_Device->WaitIdle();
    }
}
