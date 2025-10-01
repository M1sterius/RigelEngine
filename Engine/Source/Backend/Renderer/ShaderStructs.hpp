#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace Rigel::Backend::Vulkan
{
    // ShaderStructs

    struct MaterialData
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

    struct DirectionalLightData
    {
        glm::vec3 Direction;

        glm::vec3 Color;
        float32_t Intensity;
    };

    struct PointLightData
    {
        glm::vec3 Position;

        float32_t Constant;
        float32_t Linear;
        float32_t Quadratic;

        glm::vec3 Color;
        float32_t Intensity;
    };

    struct SpotLightData
    {
        glm::vec3 Position;
        glm::vec3 Direction;

        float32_t CutOff;
        float32_t OuterCutOff;

        float32_t Constant;
        float32_t Linear;
        float32_t Quadratic;

        glm::vec3 Color;
        float32_t Intensity;
    };

    struct SceneData
    {
        MeshData Meshes[2048];

        DirectionalLightData DirLights[4];
        PointLightData PointLights[256];
        SpotLightData SpotLights[256];
    };
}
