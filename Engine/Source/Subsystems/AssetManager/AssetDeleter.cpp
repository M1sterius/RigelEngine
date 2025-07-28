#include "AssetDeleter.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"
#include "SubsystemGetters.hpp"

namespace Rigel::Backend
{
    AssetDeleter::AssetDeleter(RigelAsset* ptr)
        : m_Asset(ptr) { }

    AssetDeleter::~AssetDeleter()
    {
        if (!m_Asset->m_IsPersistent)
            GetAssetManager()->Unload(m_Asset->GetID());
    }
}
