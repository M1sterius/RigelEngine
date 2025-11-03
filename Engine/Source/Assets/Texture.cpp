#include "Assets/Texture.hpp"
#include "Utilities/ScopeGuard.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Backend/Renderer/Vulkan/AssetBackends/VK_Texture.hpp"

#include "stb_image/stb_image.h"


namespace Rigel
{
    Texture::Texture(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Texture::~Texture() = default;

    ErrorCode Texture::Init()
    {
        stbi_set_flip_vertically_on_load(true);

        const auto metadata = GetAssetManager()->GetMetadata<TextureMetadata>(this->GetPath());

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        if (metadata->Path.empty() && !metadata->Pixels)
        {
            Debug::Error("Texture metadata must have either path or pixels pointer set to valid values!");
            return ErrorCode::INVALID_ASSET_METADATA;
        }

        if (!metadata->Path.empty() && metadata->Pixels)
        {
            Debug::Error("Texture metadata must not have both path and pixels fields set at the same time!");
            return ErrorCode::INVALID_ASSET_METADATA;
        }

        glm::uvec2 size;
        int32_t components;
        stbi_uc* pixels;

        if (!metadata->Path.empty()) // Load texture from file
        {
            int width, height;
            if (!stbi_info(m_Path.string().c_str(), &width, &height, &components))
                return ErrorCode::FAILED_TO_OPEN_FILE;

            // Force 4 channels if the image has only RGB (most GPUs don't support RGB)
            const bool needsAlpha = (components == 3);
            const int desiredComponents = needsAlpha ? STBI_rgb_alpha : 0;

            pixels = stbi_load(m_Path.string().c_str(), &width, &height, &components, desiredComponents);

            if (!pixels)
                return ErrorCode::FAILED_TO_OPEN_FILE;

            if (needsAlpha)
                components = 4;

            size = glm::uvec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
        else // Load texture from existing pixels buffer
        {
            if (metadata->Width == 0 || metadata->Height == 0 || metadata->Components == 0)
                return ErrorCode::INVALID_ASSET_METADATA;

            pixels = metadata->Pixels;
            size = glm::uvec2(metadata->Width, metadata->Height);
            components = metadata->Components;
        }

        const auto mipLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(size.x, size.y))) + 1);
        m_Impl = std::make_unique<Backend::Vulkan::VK_Texture>(pixels, size, components, metadata->Linear, mipLevelCount);

        if (!metadata->Path.empty())
            stbi_image_free(pixels);

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
