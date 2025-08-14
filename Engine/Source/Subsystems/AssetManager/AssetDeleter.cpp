#include "Subsystems/AssetManager/AssetDeleter.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/SubsystemGetters.hpp"

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
