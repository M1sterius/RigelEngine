#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace Rigel::Backend::Vulkan
{
    struct PushConstantData
    {
        uint32_t MeshIndex;
    };

    struct alignas(16) MaterialData
    {
        uint32_t DiffuseIndex;
        uint32_t SpecularIndex;
        uint32_t NormalsIndex;
        float Roughness;
    };

    struct alignas(16) MeshData
    {
        uint32_t MaterialIndex;
        float _pad0[3]; // 12 bytes

        glm::mat4 MVP;
        glm::mat4 ModelMat;
        glm::mat4 NormalMat;
    };

    struct alignas(16) SceneData
    {
        static constexpr uint32_t MAX_MATERIALS_ARRAY_SIZE = 1024;
        static constexpr uint32_t MAX_MESH_DATA_ARRAY_SIZE = 2048;

        MaterialData Materials[MAX_MATERIALS_ARRAY_SIZE];
        MeshData Meshes[MAX_MESH_DATA_ARRAY_SIZE];
    };
}
