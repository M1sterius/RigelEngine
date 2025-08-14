#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "AssetMetadata.hpp"

#include <array>
#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Shader;
        class VK_ShaderModule;
    }

    struct ShaderMetadata : public AssetMetadata
    {
        ~ShaderMetadata() override = default;

        std::filesystem::path VertPath;
        std::filesystem::path FragPath;
    };

    struct ShaderMetadata2 : public AssetMetadata
    {
        struct Variant
        {
            uint8_t VertexIndex;
            uint8_t FragmentIndex;
        };

        static constexpr uint32_t MAX_PATHS = 16;

        ~ShaderMetadata2() override = default;

        void AddVariant(const std::string& name, const uint8_t vertIndex, const uint8_t fragIndex)
        {
            ASSERT(vertIndex < MAX_PATHS, "Invalid shader metadata vertex path index!");
            ASSERT(fragIndex < MAX_PATHS, "Invalid shader metadata fragment path index!");
            ASSERT(!m_Variants.contains(name), "Shader variant with that name already exists!");

            m_Variants[name] = {vertIndex, fragIndex};
        }

        std::array<std::filesystem::path, MAX_PATHS> Paths;
    private:
        std::unordered_map<std::string, Variant> m_Variants;
    };

    class Shader final : public RigelAsset
    {
    public:
        ~Shader() override;
    INTERNAL:
        struct Variant
        {
            Ref<Backend::Vulkan::VK_ShaderModule> VertexModule;
            Ref<Backend::Vulkan::VK_ShaderModule> FragmentModule;
        };

        // NODISCARD Variant GetVariant(const std::string& name);

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
