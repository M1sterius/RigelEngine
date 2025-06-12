#include "Model.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Model.hpp"

namespace Rigel
{
    Model::Model(const std::filesystem::path& path) noexcept : RigelAsset(path) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        try {
            m_BackendModel = std::make_unique<Backend::Vulkan::VK_Model>(m_Path);
        }
        catch (const std::exception&) {
            return ErrorCode::FAILED_TO_CREATE_ASSET_BACKEND;
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
