#include "Assets/Material.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "ShaderStructs.hpp"
#include "VK_BindlessManager.hpp"
#include "VK_Texture.hpp"
#include "VulkanUtility.hpp"

// TODO: this constants should probably be defined in a better place
#define ERROR_TEXTURE_BINDLESS_INDEX 0
#define BLACK_TEXTURE_BINDLESS_INDEX 1

namespace Rigel
{
    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }

    ErrorCode Material::Init()
    {
        m_Data = std::make_unique<Backend::Vulkan::MaterialData>();

        auto assetManager = GetAssetManager();
        const auto metadata = assetManager->GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata->DiffusePath.empty())
            m_Diffuse = assetManager->Load<Texture>(metadata->DiffusePath);
        if (!metadata->SpecularPath.empty())
            m_Specular = assetManager->Load<Texture>(metadata->SpecularPath);
        if (!metadata->NormalsPath.empty())
            m_Normals = assetManager->Load<Texture>(metadata->NormalsPath);

        auto SetIndex = [](const AssetHandle<Texture>& texture, const uint32_t fallbackIndex) -> uint32_t
        {
            if (texture.IsNull())
                return fallbackIndex;

            texture->WaitReady();

            if (texture->IsOK())
                return texture->GetImpl()->GetBindlessIndex();

            return fallbackIndex;
        };

        m_Data->DiffuseIndex = SetIndex(m_Diffuse, ERROR_TEXTURE_BINDLESS_INDEX);
        m_Data->SpecularIndex = SetIndex(m_Specular, BLACK_TEXTURE_BINDLESS_INDEX);
        m_Data->NormalsIndex = SetIndex(m_Normals, BLACK_TEXTURE_BINDLESS_INDEX);
        m_Data->Roughness = 0.0f;

        m_BindlessIndex = Backend::Vulkan::GetVKRenderer().GetBindlessManager().AddMaterial(m_Data.get());

        return ErrorCode::OK;
    }

    Material::~Material()
    {
        Backend::Vulkan::GetVKRenderer().GetBindlessManager().RemoveMaterial(m_BindlessIndex);
    }
}
