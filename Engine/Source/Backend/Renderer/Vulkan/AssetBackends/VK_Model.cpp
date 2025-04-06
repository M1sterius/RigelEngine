#include "VK_Model.hpp"

#include "Debug.hpp"

namespace rge::backend
{
    VK_Model::VK_Model(const std::filesystem::path& path) : BackendModel(path)
    {
        Debug::Message("Loading a model at path: {}!", path.string());
    }

    VK_Model::~VK_Model()
    {
        Debug::Message("Unloading a model!");
    }
}
