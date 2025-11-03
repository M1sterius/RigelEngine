#include "Assets/Material.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Backend/Renderer/ShaderStructs.hpp"
#include "Backend/Renderer/Vulkan/VK_BindlessManager.hpp"
#include "Backend/Renderer/Vulkan/AssetBackends/VK_Texture.hpp"
#include "Backend/Renderer/Vulkan/Helpers/VulkanUtility.hpp"

#include <filesystem>

namespace Rigel
{
    using namespace Backend::Vulkan;

    static AssetHandle<Texture> LoadTexture(const TextureMetadata& textureMetadata, const bool async)
    {
        const auto name = textureMetadata.Path.empty() ? Random::UUIDv4() : textureMetadata.Path;

        if (async)
            return GetAssetManager()->LoadAsync<Texture>(name, &textureMetadata);
        else
            return GetAssetManager()->Load<Texture>(name, &textureMetadata);
    }

    static uint32_t SetBindlessIndex(const AssetHandle<Texture>& texture, const uint32_t fallbackIndex)
    {
        if (texture.IsNull())
            return fallbackIndex;

        texture->WaitReady();

        if (texture->IsOK())
            return texture->GetImpl()->GetBindlessIndex();

        return fallbackIndex;
    }

    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }

    ErrorCode Material::Init()
    {
        const auto metadata = GetAssetManager()->GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        const auto permitAsync = metadata->PermitAsyncTextureLoading;

        // Albedo
        if (!metadata->AlbedoTex.Path.empty() || metadata->AlbedoTex.Pixels)
            m_AlbedoTex = LoadTexture(metadata->AlbedoTex, permitAsync);
        m_Color = metadata->Color;

        // Metallic
        if (!metadata->MetallicTex.Path.empty() || metadata->MetallicTex.Pixels)
            m_MetallicTex = LoadTexture(metadata->MetallicTex, permitAsync);
        m_Metalness = metadata->Metalness;

        // Roughness
        if (!metadata->RoughnessTex.Path.empty() || metadata->RoughnessTex.Pixels)
            m_RoughnessTex = LoadTexture(metadata->RoughnessTex, permitAsync);
        m_Roughness = metadata->Roughness;

        // Normal
        if (!metadata->NormalTex.Path.empty() || metadata->NormalTex.Pixels)
            m_NormalTex = LoadTexture(metadata->NormalTex, permitAsync);

        // Ambient occlusion
        if (!metadata->AmbientOcclusionTex.Path.empty() || metadata->AlbedoTex.Pixels)
            m_AmbientOcclusionTex = LoadTexture(metadata->AmbientOcclusionTex, permitAsync);

        m_Tiling = metadata->Tiling;
        m_Offset = metadata->Offset;
        m_TwoSided = metadata->TwoSided;
        m_HasTransparency = metadata->HasTransparency;

        auto shaderData = MaterialData{
            .AlbedoIndex = SetBindlessIndex(m_AlbedoTex, 0),
            .Color = m_Color,
            .MetallicIndex = SetBindlessIndex(m_MetallicTex, 1),
            .Metalness = m_Metalness,
            .RoughnessIndex = SetBindlessIndex(m_RoughnessTex, 1),
            .Roughness = m_Roughness,
            .NormalIndex = SetBindlessIndex(m_NormalTex, 1),
            .AmbientOcclusionIndex = SetBindlessIndex(m_AmbientOcclusionTex, 1),
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
