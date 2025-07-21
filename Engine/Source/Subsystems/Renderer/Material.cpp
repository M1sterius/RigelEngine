#include "Material.hpp"
#include "VK_BindlessManager.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"
#include "ShaderStructs.hpp"
#include "VK_Texture.hpp"
#include "VulkanUtility.hpp"
#include "VK_BindlessManager.hpp"

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

        auto& assetManager = Engine::Get().GetAssetManager();
        const auto metadata = assetManager.GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata->DiffusePath.empty())
            m_Diffuse = assetManager.LoadAsync<Texture>(metadata->DiffusePath);
        if (!metadata->SpecularPath.empty())
            m_Specular = assetManager.LoadAsync<Texture>(metadata->SpecularPath);
        if (!metadata->NormalsPath.empty())
            m_Normals = assetManager.LoadAsync<Texture>(metadata->NormalsPath);

        auto SetIndex = [](const AssetHandle<Texture>& texture, const uint32_t fallbackIndex) -> uint32_t
        {
            if (texture.IsNull())
                return fallbackIndex;

            texture->WaitReady();

            if (texture->IsOK())
                return texture->GetImpl()->GetBindlessIndex();

            return fallbackIndex;
        };

        // m_Data->DiffuseIndex = SetIndex(m_Diffuse, ERROR_TEXTURE_BINDLESS_INDEX);
        // m_Data->SpecularIndex = SetIndex(m_Specular, BLACK_TEXTURE_BINDLESS_INDEX);
        // m_Data->NormalsIndex = SetIndex(m_Normals, BLACK_TEXTURE_BINDLESS_INDEX);
        // m_Data->Roughness = 0.0f;
        //
        // Debug::Message("{}, {}, {}", m_Data->DiffuseIndex, m_Data->SpecularIndex, m_Data->NormalsIndex);
        // m_BindlessIndex = GetBindlessManager().AddMaterial(m_Data.get());
        return ErrorCode::OK;
    }

    Material::~Material()
    {
        // GetBindlessManager().RemoveMaterial(m_BindlessIndex);
    }
}
