#include "Assets/Shader.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VK_ShaderModule.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Utilities/Filesystem/File.hpp"

#include <ranges>

namespace Rigel
{
    Shader::Shader(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Shader::~Shader() = default;

    ErrorCode Shader::Init()
    {
        const auto metadata = GetAssetManager()->GetMetadata<ShaderMetadata>(this->GetPath());

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        for (const auto& modulePath : metadata->Paths)
        {
            if (!modulePath.empty())
            {
                const auto spirv = File::ReadBinary(modulePath);
                if (spirv.IsError())
                    return spirv.GetError();

                m_ShaderModules.emplace_back(std::make_unique<Backend::Vulkan::VK_ShaderModule>(spirv.Value()));
            }
        }

        m_Variants = metadata->Variants;

        for (auto& [vertIndex, fragIndex] : m_Variants | std::views::values)
        {
            ASSERT(vertIndex < m_ShaderModules.size() && m_ShaderModules.at(vertIndex), "Invalid shader variant vertex module index!");
            ASSERT(fragIndex < m_ShaderModules.size() && m_ShaderModules.at(fragIndex), "Invalid shader variant fragment module index!");

            m_ShaderModules[vertIndex]->SetStage(Backend::ShaderStage::Vertex);
            m_ShaderModules[fragIndex]->SetStage(Backend::ShaderStage::Fragment);
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }

    Shader::Variant Shader::GetVariant(const std::string& name)
    {
        ASSERT(m_Variants.contains(name), "Invalid shader variant name!");

        const auto [vertIndex, fragIndex] = m_Variants[name];

        return {
            .VertexModule = m_ShaderModules[vertIndex].get(),
            .FragmentModule = m_ShaderModules[fragIndex].get()
        };
    }
}
