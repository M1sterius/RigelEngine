#pragma once

#include "ComponentHandle.hpp"
#include "Camera.hpp"
#include "ModelRenderer.hpp"

#include <vector>

namespace Rigel::Backend
{
    struct SceneRenderInfo
    {
        ComponentHandle<Camera> MainCamera;
        std::vector<ComponentHandle<ModelRenderer>> Models;

        inline void Reset()
        {
            MainCamera = ComponentHandle<Camera>::Null();
            Models.clear();
        }
    };
}
