#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Subsystems/AssetManager/AssetMetadata.hpp"

#include <array>
#include <memory>
#include <vector>
#include <filesystem>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_ShaderModule;
    }

    struct ShaderMetadata : public AssetMetadata
    {
        struct VariantIndices
        {
            uint8_t VertexIndex;
            uint8_t FragmentIndex;
        };

        static constexpr uint32_t MAX_PATHS = 16;

        ~ShaderMetadata() override = default;

        void AddVariant(const std::string& name, const uint8_t vertIndex, const uint8_t fragIndex)
        {
            ASSERT(vertIndex < MAX_PATHS, "Invalid shader metadata vertex path index!");
            ASSERT(fragIndex < MAX_PATHS, "Invalid shader metadata fragment path index!");
            ASSERT(!Variants.contains(name), "Shader variant with that name already exists!");

            Variants[name] = {vertIndex, fragIndex};
        }

        std::array<std::filesystem::path, MAX_PATHS> Paths;
        std::unordered_map<std::string, VariantIndices> Variants;
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

        NODISCARD Variant GetVariant(const std::string& name);
    private:
        Shader(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        std::vector<std::unique_ptr<Backend::Vulkan::VK_ShaderModule>> m_ShaderModules;
        std::unordered_map<std::string, ShaderMetadata::VariantIndices> m_Variants;

        friend class AssetManager;
    };
}
