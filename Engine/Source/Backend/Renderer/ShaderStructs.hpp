#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace Rigel::Backend::Vulkan
{
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
        static constexpr uint32_t MAX_MESHES = 2048;
        static constexpr uint32_t MAX_DIR_LIGHTS = 4;
        static constexpr uint32_t MAX_POINT_LIGHTS = 256;
        static constexpr uint32_t MAX_SPOT_LIGHTS = 256;

        glm::vec3 CameraPosition;
        uint32_t MeshCount;
        // uint32_t DirLightCount;
        // uint32_t PointLightCount;
        // uint32_t SpotLightCount;

        MeshData Meshes[MAX_MESHES];
        // DirectionalLightData DirLights[MAX_DIR_LIGHTS];
        // PointLightData PointLights[MAX_POINT_LIGHTS];
        // SpotLightData SpotLights[MAX_SPOT_LIGHTS];
    };
}
