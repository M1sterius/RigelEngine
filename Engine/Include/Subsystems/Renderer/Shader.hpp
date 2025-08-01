#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetMetadata.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Shader;
    }

    struct ShaderMetadata : public AssetMetadata
    {
        ~ShaderMetadata() override = default;

        std::filesystem::path VertPath;
        std::filesystem::path FragPath;
    };

    class Shader final : public RigelAsset
    {
    public:
        ~Shader() override;
    INTERNAL:
        NODISCARD inline Ref<Backend::Vulkan::VK_Shader> GetImpl() const
        {
            this->WaitReady();
            return m_Impl.get();
        }
    private:
        Shader(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        std::unique_ptr<Backend::Vulkan::VK_Shader> m_Impl;

        friend class AssetManager;
    };
}
