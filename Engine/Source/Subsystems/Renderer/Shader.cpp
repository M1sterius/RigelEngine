#include "Shader.hpp"
#include "Renderer.hpp"
#include "VK_Shader.hpp"
#include "File.hpp"

namespace Rigel
{
    Shader::Shader(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Shader::~Shader() = default;

    ErrorCode Shader::Init()
    {
        const auto shaderName = std::filesystem::path(m_Path).replace_extension("");

        const auto vertPath = std::filesystem::path(shaderName).concat(".vert.spv");
        const auto fragPath = std::filesystem::path(shaderName).concat(".frag.spv");

        const auto vertBytes = File::ReadBinary(vertPath);
        const auto fragBytes = File::ReadBinary(fragPath);

        if (vertBytes.IsError())
            return vertBytes.GetError();
        if (fragBytes.IsError())
            return fragBytes.GetError();

        m_Impl = std::make_unique<Backend::Vulkan::VK_Shader>(vertBytes.Value(), fragBytes.Value());

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
