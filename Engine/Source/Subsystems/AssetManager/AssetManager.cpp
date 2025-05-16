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

    uid_t AssetManager::AssignID(RigelAsset* ptr)
    {
        using namespace Backend::HandleValidation;

        const auto ID = m_NextID++;
        ptr->OverrideID(ID);
        HandleValidator::AddHandle<HandleType::AssetHandle>(ID);

        return ID;
    }

    void AssetManager::UnloadAllAssets()
    {
        m_AssetsRegistry.clear();
    }
}
