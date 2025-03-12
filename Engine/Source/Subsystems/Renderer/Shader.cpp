#include "Shader.hpp"
#include "Renderer/BackendShader.hpp"

namespace rge
{
    Shader::Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath)
        : RigelAsset(NULL_ID)
    {
        this->m_Path = vertPath;
    }

    Shader::~Shader()
    {

    }
}
