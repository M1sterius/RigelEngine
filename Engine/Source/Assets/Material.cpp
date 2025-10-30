#include "Assets/Material.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Backend/Renderer/ShaderStructs.hpp"
#include "Backend/Renderer/Vulkan/VK_BindlessManager.hpp"
#include "Backend/Renderer/Vulkan/AssetBackends/VK_Texture.hpp"
#include "../Backend/Renderer/Vulkan/Helpers/VulkanUtility.hpp"

#include <filesystem>

namespace Rigel
{
    using namespace Backend::Vulkan;

    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }

    ErrorCode Material::Init()
    {
        const auto metadata = GetAssetManager()->GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        auto LoadTexture = [metadata](const std::filesystem::path& path) -> AssetHandle<Texture>
        {
            auto textureMetadata = TextureMetadata();
            textureMetadata.Path = path;

            if (metadata->PermitAsyncTextureLoading)
                return GetAssetManager()->LoadAsync<Texture>(path, &textureMetadata);
            else
                return GetAssetManager()->Load<Texture>(path, &textureMetadata);
        };

        auto SetIndex = [](const AssetHandle<Texture>& texture, const uint32_t fallbackIndex) -> uint32_t
        {
            if (texture.IsNull())
                return fallbackIndex;

            texture->WaitReady();

            if (texture->IsOK())
                return texture->GetImpl()->GetBindlessIndex();

            return fallbackIndex;
        };

        // Albedo
        if (!metadata->AlbedoTex.empty())
            m_AlbedoTex = LoadTexture(metadata->AlbedoTex);
        m_Color = metadata->Color;

        // Metallic
        if (!metadata->MetallicTex.empty())
            m_MetallicTex = LoadTexture(metadata->MetallicTex);
        m_Metalness = metadata->Metalness;

        // Roughness
        if (!metadata->RoughnessTex.empty())
            m_RoughnessTex = LoadTexture(metadata->RoughnessTex);
        m_Roughness = metadata->Roughness;

        // Normal
        if (!metadata->NormalTex.empty())
            m_NormalTex = LoadTexture(metadata->NormalTex);

        // Ambient occlusion
        if (!metadata->AmbientOcclusionTex.empty())
            m_AmbientOcclusionTex = LoadTexture(metadata->AmbientOcclusionTex);

        m_Tiling = metadata->Tiling;
        m_Offset = metadata->Offset;
        m_TwoSided = metadata->TwoSided;
        m_HasTransparency = metadata->HasTransparency;

        auto shaderData = MaterialData{
            .AlbedoIndex = SetIndex(m_AlbedoTex, 0),
            .Color = m_Color,
            .MetallicIndex = SetIndex(m_MetallicTex, 1),
            .Metalness = m_Metalness,
            .RoughnessIndex = SetIndex(m_RoughnessTex, 1),
            .Roughness = m_Roughness,
            .NormalIndex = SetIndex(m_NormalTex, 1),
            .AmbientOcclusionIndex = SetIndex(m_AmbientOcclusionTex, 1),
            .Tiling = m_Tiling,
            .Offset = m_Offset
        };

        m_BindlessIndex = GetVKRenderer().GetBindlessManager().AddMaterial(&shaderData);

        return ErrorCode::OK;
    }

    Material::~Material()
    {
        GetVKRenderer().GetBindlessManager().RemoveMaterial(m_BindlessIndex);
    }
}
