#include "Texture.hpp"
#include "VK_Texture.hpp"

namespace Rigel
{
    Texture::Texture(const std::filesystem::path& path) noexcept : RigelAsset(path) { }
    Texture::~Texture() = default;

    ErrorCode Texture::Init()
    {
        try
        {
            m_BackendTexture = std::make_unique<Backend::Vulkan::VK_Texture>(m_Path);
        }
        catch (const std::exception&)
        {
            return ErrorCode::FAILED_TO_CREATE_ASSET_BACKEND;
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
