#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Texture2D.hpp"

#include <filesystem>

namespace Rigel
{
    class Material final : public RigelAsset
    {
    public:
        ~Material() override;
    INTERNAL:
        NODISCARD uint32_t GetBindlessIndex() const { return m_BindlessIndex; }
    private:
        Material(const std::filesystem::path& path, const uid_t id);
        ErrorCode Init() override;

        AssetHandle<Texture2D> m_AlbedoTex{};
        glm::vec3 m_Color{1.0};

        AssetHandle<Texture2D> m_MetallicTex{};
        float32_t m_Metalness{0.0};

        AssetHandle<Texture2D> m_RoughnessTex{};
        float32_t m_Roughness{1.0};

        AssetHandle<Texture2D> m_NormalTex{};
        AssetHandle<Texture2D> m_AmbientOcclusionTex{};

        glm::vec2 m_Tiling{1.0};
        glm::vec2 m_Offset{0.0};

        bool m_TwoSided{false};

        uint32_t m_BindlessIndex = UINT32_MAX;

        friend class AssetManager;
    };
}
