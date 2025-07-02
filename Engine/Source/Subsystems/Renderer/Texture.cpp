#include "Texture.hpp"
#include "VK_Texture.hpp"

namespace Rigel
{
    Texture::Texture(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Texture::~Texture() = default;

    ErrorCode Texture::Init()
    {
        try
        {
            m_Impl = std::make_unique<Backend::Vulkan::VK_Texture>(m_Path);
        }
        catch (const std::exception&)
        {
            return ErrorCode::FAILED_TO_CREATE_ASSET_BACKEND;
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }

    glm::uvec2 Texture::GetSize() const
    {
        return m_Impl->GetSize();
    }

}
