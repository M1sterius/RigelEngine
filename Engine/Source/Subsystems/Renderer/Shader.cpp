#include "Shader.hpp"
#include "Renderer/BackendShader.hpp"

namespace rge
{
    Shader::Shader(const std::filesystem::path& path)
        : RigelAsset(path)
    {
        Debug::Message("Loading a shader at path: {}", path.string());
    }

    Shader::~Shader()
    {
        Debug::Message("Destroying shader!");
    }
}
