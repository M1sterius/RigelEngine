#include "Shader.hpp"

#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Shader.hpp"

namespace Rigel
{
    Shader::Shader(const std::filesystem::path& path)
        : RigelAsset(path)
    {
        // Loading code assumes the following naming convention for shader files:
        // <shader_name>.<vert/frag>.spv

        const auto shaderName = std::filesystem::path(path).replace_extension("");

        const auto vertPath = std::filesystem::path(shaderName).concat(".vert.spv");
        const auto fragPath = std::filesystem::path(shaderName).concat(".frag.spv");

        const auto& renderer = Engine::Get().GetRenderer();

        if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::Vulkan)
            m_BackendShader = std::make_unique<Backend::Vulkan::VK_Shader>(vertPath, fragPath);
        else if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::OpenGL) { }
    }

    Shader::~Shader() = default;
}
