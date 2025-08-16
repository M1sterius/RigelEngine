#pragma once

#include "Core.hpp"
#include "AssetMetadata.hpp"

#include <array>
#include <unordered_map>

namespace Rigel
{
    struct ShaderMetadata : public AssetMetadata
    {
        struct VariantIndices
        {
            uint8_t VertexIndex;
            uint8_t FragmentIndex;
        };

        static constexpr uint32_t MAX_PATHS = 16;

        ~ShaderMetadata() override = default;

        void AddVariant(const std::string& name, const uint8_t vertIndex, const uint8_t fragIndex)
        {
            ASSERT(vertIndex < MAX_PATHS, "Invalid shader metadata vertex path index!");
            ASSERT(fragIndex < MAX_PATHS, "Invalid shader metadata fragment path index!");
            ASSERT(!Variants.contains(name), "Shader variant with that name already exists!");

            Variants[name] = {vertIndex, fragIndex};
        }

        std::array<std::filesystem::path, MAX_PATHS> Paths;
        std::unordered_map<std::string, VariantIndices> Variants;
    };
}