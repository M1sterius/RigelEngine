#include "AssetDeleter.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel::Backend
{
    AssetDeleter::AssetDeleter(RigelAsset* ptr)
        : m_Asset(ptr) { }

    AssetDeleter::~AssetDeleter()
    {
        auto& manager = Engine::Get().GetAssetManager();

        if (!m_Asset->m_IsPersistent)
            manager.Unload(m_Asset->GetID());
    }
}
