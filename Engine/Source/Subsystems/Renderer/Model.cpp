#include "Model.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Model.hpp"

namespace rge
{
    Model::Model(const std::filesystem::path& path) : RigelAsset(path)
    {
        const auto& renderer = Engine::Get().GetRenderer();

        if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::Vulkan)
            m_BackendModel = std::make_unique<backend::VK_Model>(path);
        else if (renderer.GetSelectedGraphicsAPI() == GraphicsApi::OpenGL) { }
    }

    Model::~Model() = default;
}
