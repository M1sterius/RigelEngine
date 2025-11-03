#include "VK_GPUScene.hpp"
#include "Wrapper/VulkanWrapper.hpp"
#include "Helpers/VulkanUtility.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "../ShaderStructs.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_GPUScene::VK_GPUScene(VK_Device& device, VK_Swapchain& swapchain)
        : m_Device(device), m_Swapchain(swapchain), m_DescriptorSetLayout(nullptr)
    {
        Debug::Trace("Initializing GPU scene.");

        m_SceneData = std::make_unique<SceneData>();

        for (uint32_t i = 0; i < m_Swapchain.GetFramesInFlightCount(); ++i)
        {
            m_Buffers.emplace_back(std::make_unique<VK_MemoryBuffer>(m_Device, sizeof(SceneData),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU));
        }

        CreateDescriptorSet();
    }

    VK_GPUScene::~VK_GPUScene()
    {
        Debug::Trace("Destroying GPU scene.");

        vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
    }

    void VK_GPUScene::Update(const uint32_t frameIndex)
    {
        const auto sceneRenderInfo = GetRenderer()->GetSceneRenderInfo();

        m_DeferredDrawBatches.clear();
        m_ForwardDrawBatches.clear();

        // Skip rendering altogether if there is no main camera
        if (!sceneRenderInfo->CameraPresent)
            return;

        // Process models
        uint32_t meshIndex = 0;
        for (uint32_t i = 0; i < sceneRenderInfo->Models.size(); ++i)
        {
            const auto& modelAsset = sceneRenderInfo->Models[i];
            const auto& modelTransform = sceneRenderInfo->Transforms[i];

            auto deferredBatch = DrawBatch();
            deferredBatch.VertexBuffer = modelAsset->GetVertexBuffer();
            deferredBatch.IndexBuffer = modelAsset->GetIndexBuffer();

            auto forwardBatch = DrawBatch();
            forwardBatch.VertexBuffer = modelAsset->GetVertexBuffer();
            forwardBatch.IndexBuffer = modelAsset->GetIndexBuffer();

            int32_t vertexOffset = 0;
            for (auto nodeIt = modelAsset->GetNodeIterator(); nodeIt.Valid(); nodeIt++)
            {
                const auto modelMat = modelTransform * nodeIt->WorldTransform;
                const auto normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
                const auto MVP = sceneRenderInfo->ProjView * modelMat;

                for (const auto& mesh : nodeIt->Meshes)
                {
                    const auto meshMaterial = mesh.Material;

                    m_SceneData->Meshes[meshIndex] = MeshData{
                        .MaterialIndex = mesh.Material->GetBindlessIndex(),
                        .MVP = MVP,
                        .Model = modelMat,
                        .Normal = normalMat,
                    };

                    // Separate draw calls into those that need to be done in forward pass (e.g. transparent objects)
                    // and those that can be done in deferred pass
                    if (meshMaterial->HasTransparency())
                    {
                        forwardBatch.DrawCalls.push_back({
                            .MeshIndex = meshIndex,
                            .IndexCount = mesh.IndexCount,
                            .FirstIndex = mesh.FirstIndex,
                            .VertexOffset = vertexOffset
                        });
                    }
                    else
                    {
                        deferredBatch.DrawCalls.push_back({
                            .MeshIndex = meshIndex,
                            .IndexCount = mesh.IndexCount,
                            .FirstIndex = mesh.FirstIndex,
                            .VertexOffset = vertexOffset
                        });
                    }

                    vertexOffset = (int32_t)mesh.FirstVertex + (int32_t)mesh.VertexCount;
                    meshIndex++;
                }
            }

            if (!deferredBatch.DrawCalls.empty())
                m_DeferredDrawBatches.push_back(deferredBatch);

            if (!forwardBatch.DrawCalls.empty())
                m_ForwardDrawBatches.push_back(forwardBatch);
        }

        m_SceneData->MeshCount = meshIndex;
        m_SceneData->CameraPosition = sceneRenderInfo->CamPos;
        // lights, other graphics objects?

        // Update current frame's mesh buffer
        const auto& buffer = m_Buffers[frameIndex];
        buffer->UploadData(0, sizeof(SceneData), m_SceneData.get());
    }

    void VK_GPUScene::CreateDescriptorSet()
    {
        // Layout creation
        std::vector<VkDescriptorSetLayoutBinding> bindings(1);

        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

        auto setLayoutInfo = MakeInfo<VkDescriptorSetLayoutCreateInfo>();
        setLayoutInfo.bindingCount = bindings.size();
        setLayoutInfo.pBindings = bindings.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.Get(), &setLayoutInfo, nullptr, &m_DescriptorSetLayout), "Failed to create descriptor set layout!");

        // Set creation
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
            bufferInfo.buffer = m_Buffers[i]->Get();
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
}
