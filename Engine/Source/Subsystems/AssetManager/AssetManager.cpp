#include "AssetManager.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "Directory.hpp"
#include "Shader.hpp"
#include "Hash.hpp"
#include "json.hpp"

#include <stdexcept>

namespace fs = std::filesystem;

namespace Rigel
{
    int32_t AssetManager::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up asset manager.");
        m_Initialized = true;

        return 0;
    }

    int32_t AssetManager::Shutdown()
    {
        Debug::Trace("Shutting down asset manager.");

        return 0;
    }

    uid_t AssetManager::GetNextAssetID()
    {
        std::unique_lock lock(m_IDMutex);
        return m_NextAssetID++;
    }

    uid_t AssetManager::AssignID(RigelAsset* ptr)
    {
        const auto ID = GetNextAssetID();
        ptr->OverrideID(ID);
        return ID;
    }

    void AssetManager::IncrementRefCount(const uid_t id)
    {
        if (id == NULL_ID) return;

        std::unique_lock lock(m_RegistryMutex);

        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
            {
                record.RefCount++;
                return;
            }
        }

        Debug::Error("Failed to increment reference count for an asset with ID {}. Asset cannot be found!", id);
    }

    void AssetManager::DecrementRefCount(const uid_t id)
    {
        if (id == NULL_ID) return;

        std::unique_lock lock(m_RegistryMutex);

        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
            {
                record.RefCount--;
                return;
            }
        }

        Debug::Error("Failed to decrement reference count for an asset with ID {}. Asset cannot be found!", id);
    }

    uint32_t AssetManager::GetRefCount(const uid_t id)
    {
        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
                return record.RefCount;
        }

        return -1;
    }

    void AssetManager::UnloadAllAssets()
    {
        m_AssetsRegistry.clear();
    }
}
