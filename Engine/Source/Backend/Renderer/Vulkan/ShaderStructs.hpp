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
        uint32_t AlbedoIndex{0};
        glm::vec3 Color{1.0};

        uint32_t MetallicIndex{1};
        float32_t Metalness{0.0};

        uint32_t RoughnessIndex{1};
        float32_t Roughness{1.0};

        uint32_t NormalIndex{1};
        uint32_t AmbientOcclusionIndex{1};

        glm::vec2 Tiling{1.0};
        glm::vec2 Offset{0.0};
    };

    struct MeshData
    {
        uint32_t MaterialIndex;

        glm::mat4 MVP;
        glm::mat4 Model;
        glm::mat3 Normal;
    };

    struct SceneData
    {
        static constexpr uint32_t MAX_MATERIALS_ARRAY_SIZE = 1024;
        static constexpr uint32_t MAX_MESH_DATA_ARRAY_SIZE = 2048;

        MaterialData Materials[MAX_MATERIALS_ARRAY_SIZE];
        MeshData Meshes[MAX_MESH_DATA_ARRAY_SIZE];
    };
}
