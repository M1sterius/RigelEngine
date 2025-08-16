#pragma once

#include "AssetMetadata.hpp"

#include <filesystem>

namespace Rigel
{
    struct MaterialMetadata final : AssetMetadata
    {
        std::filesystem::path DiffusePath{};
        std::filesystem::path SpecularPath{};
        std::filesystem::path NormalsPath{};
    };
}