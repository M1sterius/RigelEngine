#include "VK_Model.hpp"
#include "obj_loader.h"
#include "Debug.hpp"

#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Device.hpp"
#include "VK_Renderer.hpp"
#include "VK_VertexBuffer.hpp"
#include "VK_IndexBuffer.hpp"

namespace Rigel::Backend
{
    VK_Model::VK_Model(const std::filesystem::path& path) : BackendModel(path)
    {
        if (!IsFormatSupported(m_Path.extension().string()))
            throw RigelException("Attempted to a load a model in an unsupported format!");

        // TODO: Rework to use assimp. Add support for multimesh models with materials

        auto loader = objl::Loader();

        if (!loader.LoadFile(m_Path.string()))
            throw RigelException("Failed to load model at path: " + m_Path.string());

        auto vertices = std::vector<Vertex>(loader.LoadedVertices.size());

        for (size_t i = 0; i < loader.LoadedVertices.size(); i++)
        {
            const auto& loaderVertex = loader.LoadedVertices[i];

            const auto position = glm::vec3(loaderVertex.Position.X, loaderVertex.Position.Y, loaderVertex.Position.Z);
            const auto texCoords = glm::vec2(loaderVertex.TextureCoordinate.X, loaderVertex.TextureCoordinate.Y);

            vertices[i] = Vertex(position, texCoords);
        }

        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = dynamic_cast<const VK_Renderer&>(renderer.GetBackend());
        auto& device = backend.GetDevice();

        m_VertexBuffer = std::make_unique<VK_VertexBuffer>(device, vertices);
        m_IndexBuffer = std::make_unique<VK_IndexBuffer>(device, loader.LoadedIndices);
    }

    VK_Model::~VK_Model()
    {

    }

    bool VK_Model::IsFormatSupported(const std::string_view& extension)
    {
        if (extension == ".obj")
            return true;

        return false;
    }
}
