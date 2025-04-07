#include "AssetHandle.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel::Backend::HandleValidation
{
    bool ValidateAssetHandleImpl(const uid_t id)
    {
        const auto& assetManager = Engine::Get().GetAssetManager();
        return assetManager.Validate(id);
    }
}
