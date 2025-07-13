#pragma once

#include "Core.hpp"

namespace Rigel::Backend::Vulkan
{
    struct PushConstantData
    {
        glm::mat4 MVP;
        uint32_t MeshIndex;
    };

    struct MeshData
    {
        uint32_t DiffuseIndex;
        uint32_t SpecularIndex;
        uint32_t NormalIndex;
        uint32_t _padding0;
    };

    struct SceneData
    {
        static constexpr uint32_t MAX_MESH_DATA_ARRAY_SIZE = 4096;

        uint32_t MeshDataSize;
        uint32_t _padding0[3];
        MeshData MeshData[MAX_MESH_DATA_ARRAY_SIZE];
    };
}