#include "AssetManager.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "Directory.hpp"

#include <stdexcept>

namespace fs = std::filesystem;

namespace rge
{

    AssetManager::AssetManager() { Startup(); }

    AssetManager::~AssetManager() { Shutdown(); }

    void AssetManager::Startup()
    {
        RGE_TRACE("Starting up asset manager.");

        m_WorkingDirectory = rge::Directory::WorkingDirectory();
        m_AssetsDirectory = m_WorkingDirectory.concat("/Assets");

        if (!fs::exists(m_AssetsDirectory))
            throw RigelException("Asset manager initialisation failed! Cannot find root Assets directory in the working directory.");

        m_Initialized = true;
    }

    void AssetManager::Shutdown()
    {
        RGE_TRACE("Shutting down asset manager.");
    }
}