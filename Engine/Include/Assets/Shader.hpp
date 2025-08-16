#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"
#include "Metadata/ShaderMetadata.hpp"

#include <memory>
#include <vector>
#include <filesystem>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_ShaderModule;
    }

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
