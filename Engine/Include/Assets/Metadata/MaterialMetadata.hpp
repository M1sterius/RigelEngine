#pragma once

#include "Core.hpp"
#include "AssetMetadata.hpp"
#include "TextureMetadata.hpp"

#include <filesystem>

namespace Rigel
{
    struct MaterialMetadata : public AssetMetadata
    {
        TextureMetadata AlbedoTex{};
        glm::vec3 Color{1.0};

        TextureMetadata MetallicTex{};
        float32_t Metalness{0.0};

        TextureMetadata RoughnessTex{};
        float32_t Roughness{1.0};

        TextureMetadata NormalTex{};
        TextureMetadata AmbientOcclusionTex{};

        glm::vec2 Tiling{1.0};
        glm::vec2 Offset{0.0};

        bool TwoSided{false};
        bool HasTransparency{false};

        bool PermitAsyncTextureLoading{false};
    };
}