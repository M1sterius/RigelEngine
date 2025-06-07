#include "AssetHandle.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel::Backend::AssetHandleUtilityImpl
{
    void OnRefCountReachZero(const uid_t id)
    {
        auto& manager = Engine::Get().GetAssetManager();
        manager.Unload(id);
    }

    bool IsAssetReady(const uid_t id)
    {
        const auto& manager = Engine::Get().GetAssetManager();
        return manager.IsAssetReady(id);
    }
}
