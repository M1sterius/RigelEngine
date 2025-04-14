#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend
    {
        class ITextureBackend;
    }

    class Texture final : public RigelAsset
    {
    public:
        ~Texture() override;
    INTERNAL:
        template<typename T> requires std::is_base_of_v<Backend::ITextureBackend, T>
        NODISCARD T& GetBackend() const
        {
            return static_cast<T&>(*m_BackendTexture);
        }
    private:
        explicit Texture(const std::filesystem::path& path);

        std::unique_ptr<Backend::ITextureBackend> m_BackendTexture;

        friend class AssetManager;
    };
}
