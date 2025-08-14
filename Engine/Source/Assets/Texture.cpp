#include "Assets/Texture.hpp"
#include "Utilities/ScopeGuard.hpp"
#include "VK_Texture.hpp"

#include "stb_image/stb_image.h"

namespace Rigel
{
    Texture::Texture(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Texture::~Texture() = default;

    ErrorCode Texture::Init()
    {
        stbi_set_flip_vertically_on_load(true);

        int texWidth, texHeight, texChannels;
        auto pixels = stbi_load(m_Path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        const auto size = glm::uvec2(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        auto freeGuard = ScopeGuard([pixels] { stbi_image_free(pixels); });

        if (!pixels)
            return ErrorCode::FAILED_TO_OPEN_FILE;

        m_Impl = std::make_unique<Backend::Vulkan::VK_Texture>(pixels, size);

        m_Initialized = true;
        return ErrorCode::OK;
    }

    glm::uvec2 Texture::GetSize() const
    {
        return m_Impl->GetSize();
    }

    const Texture::SamplerProperties& Texture::GetSamplerProperties() const
    {
        return m_Impl->GetSamplerProperties();
    }
}
