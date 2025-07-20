#include "Material.hpp"
#include "VK_BindlessManager.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "AssetManager.hpp"
#include "ShaderStructs.hpp"
#include "VK_Texture.hpp"
#include "VK_Renderer.hpp"
#include "VK_BindlessManager.hpp"

// TODO: this constants should probably be defined in a better place
#define ERROR_TEXTURE_BINDLESS_INDEX 0
#define BLACK_TEXTURE_BINDLESS_INDEX 1

namespace Rigel
{
    NODISCARD static Backend::Vulkan::VK_BindlessManager& GetBindlessManager()
    {
        return Engine::Get().GetRenderer().GetImpl().GetBindlessManager();
    }

    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }

    ErrorCode Material::Init()
    {
        m_Data = std::make_unique<Backend::Vulkan::MaterialData>();

        auto& assetManager = Engine::Get().GetAssetManager();
        const auto metadata = assetManager.GetMetadata<MaterialMetadata>(this->m_Path);

        if (!metadata->DiffusePath.empty())
            m_Diffuse = assetManager.LoadAsync<Texture>(metadata->DiffusePath);
        else
            Debug::Error("Material at path {} could not load diffuse texture!", this->GetPath().string());

        if (!metadata->SpecularPath.empty())
            m_Specular = assetManager.LoadAsync<Texture>(metadata->SpecularPath);
        else
            Debug::Error("Material at path {} could not load specular texture!", this->GetPath().string());

        if (!metadata->NormalsPath.empty())
            m_Normals = assetManager.LoadAsync<Texture>(metadata->NormalsPath);
        else
            Debug::Error("Material at path {} could not load normals texture!", this->GetPath().string());

        // WaitReady is called inside GetImpl
        m_Data->DiffuseIndex = m_Diffuse->IsOK() ? m_Diffuse->GetImpl()->GetBindlessIndex() : ERROR_TEXTURE_BINDLESS_INDEX;
        m_Data->SpecularIndex = m_Specular->IsOK() ? m_Specular->GetImpl()->GetBindlessIndex() : BLACK_TEXTURE_BINDLESS_INDEX;
        m_Data->NormalsIndex = m_Normals->IsOK() ? m_Normals->GetImpl()->GetBindlessIndex() : BLACK_TEXTURE_BINDLESS_INDEX;
        m_Data->Roughness = 0.0f;

        m_BindlessIndex = GetBindlessManager().AddMaterial(m_Data.get());

        return ErrorCode::OK;
    }

    Material::~Material()
    {
        GetBindlessManager().RemoveMaterial(m_BindlessIndex);
    }
}
