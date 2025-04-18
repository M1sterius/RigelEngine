#include "AssetHandle.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel::Backend
{
    namespace RefCountingImpl
    {
        void IncrementRefCount(const uid_t id)
        {
            auto& assetManager = Engine::Get().GetAssetManager();
            assetManager.IncrementRefCount(id);
        }

        void DecrementRefCount(const uid_t id)
        {
            auto& assetManager = Engine::Get().GetAssetManager();
            assetManager.DecrementRefCount(id);
        }
    }
}

