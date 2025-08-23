#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace Rigel::Backend::Vulkan
{
    struct MaterialData
    {
        uint32_t DiffuseIndex;
        uint32_t SpecularIndex;
        uint32_t NormalsIndex;
        float Roughness;
    };

    struct PBRMaterialData
    {
        uint32_t AlbedoIndex;
        glm::vec3 Color{1.0};

        uint32_t MetallicIndex;
        glm::vec3 Metalness{0.0};

        uint32_t RoughnessIndex;
        glm::vec3 Roughness{1.0};

        uint32_t NormalIndex;
        uint32_t AmbientOcclusionIndex;
    };

    struct MeshData
    {
        uint32_t MaterialIndex;

        glm::mat4 MVP;
        glm::mat4 ModelMat;
        glm::mat3 NormalMat;
    };

    struct SceneData
    {
        static constexpr uint32_t MAX_MATERIALS_ARRAY_SIZE = 1024;
        static constexpr uint32_t MAX_MESH_DATA_ARRAY_SIZE = 2048;

        MaterialData Materials[MAX_MATERIALS_ARRAY_SIZE];
        MeshData Meshes[MAX_MESH_DATA_ARRAY_SIZE];
    };
}
