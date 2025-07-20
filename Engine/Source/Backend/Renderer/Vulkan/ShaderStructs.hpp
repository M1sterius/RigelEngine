#pragma once

#include "Core.hpp"

namespace Rigel::Backend::Vulkan
{
    struct PushConstantData
    {
        glm::mat4 MVP;
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
        float _pad0[3];
    };

    struct alignas(16) SceneData
    {
        static constexpr uint32_t MAX_MESH_DATA_ARRAY_SIZE = 4096;
        static constexpr uint32_t MAX_MATERIAL_DATA_ARRAY_SIZE = 1024;

        MaterialData Materials[MAX_MATERIAL_DATA_ARRAY_SIZE];
        MeshData Meshes[MAX_MESH_DATA_ARRAY_SIZE];
    };
}
