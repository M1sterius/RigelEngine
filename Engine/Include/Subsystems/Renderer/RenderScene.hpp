#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Assets/Model.hpp"
#include "Handles/AssetHandle.hpp"

#include <optional>

namespace Rigel
{
    class SceneHandle;

    struct RenderCamera
    {
        glm::vec3 Position;
        glm::mat4 ProjView;
    };

    struct RenderModel
    {
        AssetHandle<Model> Model;
        glm::mat4 Transform;
    };

    struct RenderDirectionalLight
    {
        glm::vec3 Direction;

        glm::vec3 Color;
        float32_t Intensity;

        bool CastShadows;
    };

    struct RenderPointLight
    {
        glm::vec3 Position;

        float32_t Constant;
        float32_t Linear;
        float32_t Quadratic;

        glm::vec3 Color;
        float32_t Intensity;
    };

    struct RenderSpotLight
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

    class RenderScene
    {
    public:
        NODISCARD static RenderScene Extract(const SceneHandle& scene);

        std::optional<RenderCamera> Camera;
        std::vector<RenderModel> Models;

        std::vector<RenderDirectionalLight> DirectionalLights;
    };
}