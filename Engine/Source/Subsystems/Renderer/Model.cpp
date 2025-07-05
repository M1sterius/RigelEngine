#include "Model.hpp"
#include "Renderer.hpp"
#include "VK_Model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Rigel
{
    Model::Model(const std::filesystem::path& path, const uid_t id) noexcept
        : RigelAsset(path, id) { }
    Model::~Model() = default;

    ErrorCode Model::Init()
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(m_Path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
            return ErrorCode::FAILED_TO_OPEN_FILE;
        }

        m_Initialized = true;
        return ErrorCode::OK;
    }
}
