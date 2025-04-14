#include "Texture.hpp"
#include "VK_Texture.hpp"

namespace Rigel
{
    Texture::Texture(const std::filesystem::path& path) : RigelAsset(path)
    {
        m_BackendTexture = std::make_unique<Backend::Vulkan::VK_Texture>(path);
    }

    Texture::~Texture() = default;
}
