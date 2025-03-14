#include "Shader.hpp"
#include "Renderer/BackendShader.hpp"

namespace rge
{
    Shader::Shader(const std::filesystem::path& path)
        : RigelAsset(path)
    {
        // Loading code assumes the following naming convention for shader files:
        // <shader_name>.<vert/frag>.spv

        const auto shaderName = std::filesystem::path(path).replace_extension("");

        const auto vertPath = std::filesystem::path(shaderName).concat(".vert.spv");
        const auto fragPath = std::filesystem::path(shaderName).concat(".frag.spv");

        Debug::Message("Load shader!");
    }

    Shader::~Shader()
    {
        Debug::Message("Delete shader!");
    }
}
