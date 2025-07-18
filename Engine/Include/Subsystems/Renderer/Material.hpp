#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetMetadata.hpp"
#include "AssetHandle.hpp"
#include "Texture.hpp"

#include <filesystem>

namespace Rigel
{
    struct MaterialMetadata final : AssetMetadata
    {
        std::filesystem::path DiffusePath{};
        std::filesystem::path SpecularPath{};
        std::filesystem::path NormalsPath{};
    };

    class Material final : public RigelAsset
    {
    public:
        ~Material() override;
    private:
        Material(const std::filesystem::path& path, const uid_t id);
        ErrorCode Init() override;

        AssetHandle<Texture> m_Diffuse;
        AssetHandle<Texture> m_Specular;
        AssetHandle<Texture> m_Normals;
    };
}
