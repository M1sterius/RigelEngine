#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ComponentHandle.hpp"
#include "Camera.hpp"
#include "ModelRenderer.hpp"
#include "AssetHandle.hpp"
#include "../../Assets/Model.hpp"

#include <vector>

namespace Rigel::Backend
{
    // Used to pass info about the scene to vulkan rendering backend
    struct SceneRenderInfo
    {
        bool CameraPresent;
        glm::mat4 ProjView;

        std::vector<AssetHandle<Model>> Models;
        std::vector<glm::mat4> Transforms;


        inline void Reset()
        {
            CameraPresent = false;
            ProjView = glm::identity<glm::mat4>();

            Models.clear();
            Transforms.clear();
        }
    };
}
