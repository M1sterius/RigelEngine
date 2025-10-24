#include "Assets/Model.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Backend/Renderer/Vulkan/Helpers/Vertex.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VK_VertexBuffer.hpp"
#include "Backend/Renderer/Vulkan/Wrapper/VK_IndexBuffer.hpp"
#include "Utilities/Loaders/GLTF_Loader.hpp"

namespace Rigel
{
    using namespace Backend::Vulkan;

    Model::Model(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        auto vertices = std::vector<Vertex3p2t3n4g>();
        auto indices = std::vector<uint32_t>();

        m_RootNode = std::make_shared<Backend::ModelNode>();

        Backend::GLTF_Loader gltfLoader;

        if (const auto result = gltfLoader.LoadModel(m_Path, m_RootNode, m_Materials, vertices, indices); !result)
        {
            Debug::Error("GLTF loading error: {}", gltfLoader.GetErrorString());
            return ErrorCode::FAILED_TO_OPEN_FILE;
        }

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(indices);

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
