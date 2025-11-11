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

    };

    struct RenderPointLight
    {

    };

    struct RenderSpotLight
    {

    };

    class RenderScene
    {
    public:
        NODISCARD static RenderScene Extract(const SceneHandle& scene);

        std::optional<RenderCamera> Camera;
        std::vector<RenderModel> Models;
    };
}