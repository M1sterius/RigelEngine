#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Texture.hpp"

#include <filesystem>

namespace Rigel
{
    class Material final : public RigelAsset
    {
    public:
        ~Material() override;
    INTERNAL:
        NODISCARD bool IsTwoSided() const { return m_TwoSided; }
        NODISCARD bool HasTransparency() const { return m_HasTransparency; }

        NODISCARD uint32_t GetBindlessIndex() const { return m_BindlessIndex; }
    private:
        Material(const std::filesystem::path& path, const uid_t id);
        ErrorCode Init() override;

        AssetHandle<Texture> m_AlbedoTex{};
        glm::vec3 m_Color{1.0};

        AssetHandle<Texture> m_MetallicTex{};
        float32_t m_Metalness{0.0};

        AssetHandle<Texture> m_RoughnessTex{};
        float32_t m_Roughness{1.0};

        AssetHandle<Texture> m_NormalTex{};
        AssetHandle<Texture> m_AmbientOcclusionTex{};

        glm::vec2 m_Tiling{1.0};
        glm::vec2 m_Offset{0.0};

        bool m_TwoSided{false};
        bool m_HasTransparency{false};

        uint32_t m_BindlessIndex = UINT32_MAX;

        friend class AssetManager;
    };
}
