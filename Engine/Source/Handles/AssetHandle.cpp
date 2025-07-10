#include "AssetHandle.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel::Backend::AssetHandleRefCountingImpl
{
    void OnRefCountReachZero(const uid_t id)
    {
        auto& manager = Engine::Get().GetAssetManager();
        manager.Unload(id);
    }
}
