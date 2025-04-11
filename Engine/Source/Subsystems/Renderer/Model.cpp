#include "Model.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Model.hpp"

namespace Rigel
{
    Model::Model(const std::filesystem::path& path) : RigelAsset(path)
    {
        m_BackendModel = std::make_unique<Backend::Vulkan::VK_Model>(path);
    }

    Model::~Model() = default;
}
