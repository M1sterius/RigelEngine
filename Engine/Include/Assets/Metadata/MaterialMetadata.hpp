#pragma once

#include "AssetMetadata.hpp"

#include <filesystem>

namespace Rigel
{
    struct MaterialMetadata : public AssetMetadata
    {
        std::filesystem::path AlbedoTex{};
        glm::vec3 Color{1.0};

        std::filesystem::path MetallicTex{};
        float32_t Metalness{0.0};

        std::filesystem::path RoughnessTex{};
        float32_t Roughness{1.0};

        std::filesystem::path NormalTex{};
        std::filesystem::path AmbientOcclusionTex{};

        glm::vec2 Tiling{1.0};
        glm::vec2 Offset{0.0};

        bool TwoSided{false};
        bool PermitAsyncTextureLoading{false};
    };
}