#pragma once

#include "Core.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/Material.hpp"

#include "tiny_gltf/tiny_gltf.h"

#include <filesystem>

namespace Rigel::Backend
{
    namespace Vulkan { struct Vertex3p2t3n4g; }

    struct ModelMesh;
    struct ModelNode;

    class GLTF_Loader
    {
    public:
        GLTF_Loader();
        ~GLTF_Loader();

        NODISCARD bool LoadModel(const std::filesystem::path& path, std::shared_ptr<ModelNode>& rootNode,
            std::vector<AssetHandle<Material>>& materials, std::vector<Vulkan::Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices);

        NODISCARD std::string GetErrorString() const { return m_LoadError; }
    private:
        std::filesystem::path m_Path;

        tinygltf::TinyGLTF m_Loader;
        tinygltf::Model m_Model;

        std::string m_LoadError;
        std::string m_LoadWarning;

        void ProcessNode(const int nodeIdx, const std::shared_ptr<ModelNode>& curNode,
            std::vector<Vulkan::Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices, std::vector<AssetHandle<Material>>& materials);

        ModelMesh ProcessMesh(const tinygltf::Primitive& primitive, std::vector<Vulkan::Vertex3p2t3n4g>& vertices,
            std::vector<uint32_t>& indices, std::vector<AssetHandle<Material>>& materials);

        AssetHandle<Material> ProcessMaterial(const int materialIdx);

        TextureMetadata ProcessTexture(const tinygltf::Image& image, const std::filesystem::path& texturesDir, const bool linear);
    };
}
