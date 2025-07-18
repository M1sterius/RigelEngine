#include "Material.hpp"
#include "VK_BindlessManager.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel
{
    Material::Material(const std::filesystem::path& path, const uid_t id)
        : RigelAsset(path, id) { }
    Material::~Material() = default;

    ErrorCode Material::Init()
    {
        auto& assetManager = Engine::Get().GetAssetManager();

        return ErrorCode::OK;
    }
}
