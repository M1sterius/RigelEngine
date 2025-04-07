#include "Model.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Model.hpp"

namespace Rigel
{
    Model::Model(const std::filesystem::path& path) : RigelAsset(path)
    {
        const auto& renderer = Engine::Get().GetRenderer();

        if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::Vulkan)
            m_BackendModel = std::make_unique<Backend::VK_Model>(path);
        else if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::OpenGL) { }
    }

    Model::~Model() = default;
}
