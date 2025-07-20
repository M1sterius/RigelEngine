#pragma once

#include "Core.hpp"

namespace Rigel
{
    class BuiltInAssets
    {
    public:
        static constexpr auto ModelCube = "Assets/Engine/Models/cube.obj";
        static constexpr auto ModelCone = "Assets/Engine/Models/cone.obj";
        static constexpr auto ModelSphere = "Assets/Engine/Models/sphere.obj";

        static constexpr auto TextureError = "Assets/Engine/Textures/TextureError.jpg";
        static constexpr auto TextureWhite = "Assets/Engine/Textures/TextureWhite.png";
        static constexpr auto TextureBlack = "Assets/Engine/Textures/TextureBlack.png";
        static constexpr auto TextureBruh = "Assets/Engine/Textures/TextureBruh.png";

        static constexpr auto ShaderDefault = "Assets/Engine/Shaders/DefaultShader.spv";
    };
}