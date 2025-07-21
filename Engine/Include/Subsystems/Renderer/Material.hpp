#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetMetadata.hpp"
#include "AssetHandle.hpp"
#include "Texture.hpp"

#include <filesystem>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        struct MaterialData;
    }

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
    INTERNAL:
        NODISCARD uint32_t GetBindlessIndex() const { return m_BindlessIndex; }
    private:
        Material(const std::filesystem::path& path, const uid_t id);
        ErrorCode Init() override;

        std::unique_ptr<Backend::Vulkan::MaterialData> m_Data;

        AssetHandle<Texture> m_Diffuse;
        AssetHandle<Texture> m_Specular;
        AssetHandle<Texture> m_Normals;

        uint32_t m_BindlessIndex = UINT32_MAX;

        friend class AssetManager;
    };
}
