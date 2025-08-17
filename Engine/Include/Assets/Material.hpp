#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Texture2D.hpp"

#include <filesystem>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        struct MaterialData;
    }

    class Material final : public RigelAsset
    {
    public:
        ~Material() override;
    INTERNAL:
        NODISCARD uint32_t GetBindlessIndex() const { return m_BindlessIndex; }
    private:
        Material(const std::filesystem::path& path, const uid_t id);
        ErrorCode Init() override;

        AssetHandle<Texture2D> m_Diffuse;
        AssetHandle<Texture2D> m_Specular;
        AssetHandle<Texture2D> m_Normals;

        uint32_t m_BindlessIndex = UINT32_MAX;
        std::unique_ptr<Backend::Vulkan::MaterialData> m_Data;

        friend class AssetManager;
    };
}
