#include "Shader.hpp"

#include "Renderer.hpp"
#include "VK_Shader.hpp"

namespace Rigel
{
    Shader::Shader(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Shader::~Shader() = default;

    ErrorCode Shader::Init()
    {
        try
        {
            // Loading code assumes the following naming convention for shader files:
            // <shader_name>.<vert/frag>.spv

            const auto shaderName = std::filesystem::path(m_Path).replace_extension("");

            const auto vertPath = std::filesystem::path(shaderName).concat(".vert.spv");
            const auto fragPath = std::filesystem::path(shaderName).concat(".frag.spv");

            m_BackendShader = std::make_unique<Backend::Vulkan::VK_Shader>(vertPath, fragPath);
        }
        catch (const std::exception&)
        {
            return ErrorCode::FAILED_TO_CREATE_ASSET_BACKEND;
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
