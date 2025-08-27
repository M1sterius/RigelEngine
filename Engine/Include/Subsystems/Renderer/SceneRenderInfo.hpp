#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Model.hpp"

#include <vector>

namespace Rigel::Backend
{
    // Used to pass info about the scene to vulkan rendering backend
    struct SceneRenderInfo
    {
        bool CameraPresent;
        glm::mat4 ProjView;
        glm::vec3 CamPos;

        std::vector<AssetHandle<Model>> Models;
        std::vector<glm::mat4> Transforms;

        void Reset()
        {
            CameraPresent = false;
            ProjView = glm::identity<glm::mat4>();

            Models.clear();
            Transforms.clear();
        }
    };
}
