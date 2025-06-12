#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Texture;
    }

    class Texture final : public RigelAsset
    {
    public:
        ~Texture() override;
    INTERNAL:
        NODISCARD Backend::Vulkan::VK_Texture& GetBackend() const { return *m_BackendTexture; }
    private:
        explicit Texture(const std::filesystem::path& path) noexcept;
        ErrorCode Init() override;

        std::unique_ptr<Backend::Vulkan::VK_Texture> m_BackendTexture;

        friend class AssetManager;
    };
}
