#include "Shader.hpp"
#include "Renderer.hpp"
#include "VK_Shader.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"
#include "File.hpp"

namespace Rigel
{
    Shader::Shader(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Shader::~Shader() = default;

    ErrorCode Shader::Init()
    {
        auto& assetManager = Engine::Get().GetAssetManager();
        const auto metadata = assetManager.GetMetadata<ShaderMetadata>(this->GetPath());

        if (!metadata)
            return ErrorCode::ASSET_METADATA_NOT_FOUND;

        const auto vertBytes = File::ReadBinary(metadata->VertPath);
        const auto fragBytes = File::ReadBinary(metadata->FragPath);

        if (vertBytes.IsError())
            return vertBytes.GetError();
        if (fragBytes.IsError())
            return fragBytes.GetError();

        m_Impl = std::make_unique<Backend::Vulkan::VK_Shader>(vertBytes.Value(), fragBytes.Value());

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
