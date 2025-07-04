#pragma once

#include "Core.hpp"

namespace Rigel
{
    class BuiltInAssets
    {
    public:
        static constexpr auto ModelCube = "Assets/EngineAssets/Models/cube.obj";
        static constexpr auto ModelCone = "Assets/EngineAssets/Models/cone.obj";
        static constexpr auto ModelSphere = "Assets/EngineAssets/Models/sphere.obj";

        static constexpr auto TextureError = "Assets/EngineAssets/Textures/TextureError.jpg";
        static constexpr auto TextureWhite = "Assets/EngineAssets/Textures/TextureWhite.png";
        static constexpr auto TextureBlack = "Assets/EngineAssets/Textures/TextureBlack.png";
        static constexpr auto TextureBruh = "Assets/EngineAssets/Textures/TextureBruh.png";

        static constexpr auto ShaderDefault = "Assets/EngineAssets/Shaders/DefaultShader.spv";
    };
}