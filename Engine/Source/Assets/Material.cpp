#include "Assets/Material.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "ShaderStructs.hpp"
#include "VK_BindlessManager.hpp"
#include "VK_Texture.hpp"
#include "VulkanUtility.hpp"

namespace Rigel
{
    using namespace Backend::Vulkan;

    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }

    ErrorCode Material::Init()
    {
        m_Data = std::make_unique<MaterialData>();

        auto assetManager = GetAssetManager();
        const auto metadata = assetManager->GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        if (!metadata->DiffusePath.empty())
            m_Diffuse = assetManager->Load<Texture2D>(metadata->DiffusePath);
        if (!metadata->SpecularPath.empty())
            m_Specular = assetManager->Load<Texture2D>(metadata->SpecularPath);
        if (!metadata->NormalsPath.empty())
            m_Normals = assetManager->Load<Texture2D>(metadata->NormalsPath);

        auto SetIndex = [](const AssetHandle<Texture2D>& texture, const uint32_t fallbackIndex) -> uint32_t
        {
            if (texture.IsNull())
                return fallbackIndex;

            texture->WaitReady();

            if (texture->IsOK())
                return texture->GetImpl()->GetBindlessIndex();

            return fallbackIndex;
        };

        m_Data->DiffuseIndex = SetIndex(m_Diffuse, VK_BindlessManager::ERROR_TEXTURE_BINDLESS_INDEX);
        m_Data->SpecularIndex = SetIndex(m_Specular, VK_BindlessManager::BLACK_TEXTURE_BINDLESS_INDEX);
        m_Data->NormalsIndex = SetIndex(m_Normals, VK_BindlessManager::BLACK_TEXTURE_BINDLESS_INDEX);
        m_Data->Roughness = 0.0f;

        m_BindlessIndex = GetVKRenderer().GetBindlessManager().AddMaterial(m_Data.get());

        return ErrorCode::OK;
    }

    Material::~Material()
    {
        GetVKRenderer().GetBindlessManager().RemoveMaterial(m_BindlessIndex);
    }
}
