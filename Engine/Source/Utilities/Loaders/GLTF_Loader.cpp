#include "GLTF_Loader.hpp"
#include "Assets/Model.hpp"
#include "Assets/Metadata/MaterialMetadata.hpp"
#include "Backend/Renderer/Vulkan/Helpers/Vertex.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

#include "tiny_gltf/tiny_gltf.h"

inline glm::mat4 ConvertMat4(const std::vector<double>& matrix)
{
    if (matrix.size() != 16)
        return glm::mat4(1.0f);

    return {
        static_cast<float>(matrix[0]), static_cast<float>(matrix[1]), static_cast<float>(matrix[2]), static_cast<float>(matrix[3]),
        static_cast<float>(matrix[4]), static_cast<float>(matrix[5]), static_cast<float>(matrix[6]), static_cast<float>(matrix[7]),
        static_cast<float>(matrix[8]), static_cast<float>(matrix[9]), static_cast<float>(matrix[10]), static_cast<float>(matrix[11]),
        static_cast<float>(matrix[12]), static_cast<float>(matrix[13]), static_cast<float>(matrix[14]), static_cast<float>(matrix[15])
    };
}

inline glm::vec3 ConvertVec3(const std::vector<double>& vec)
{
    if (vec.size() != 3)
        return glm::vec3(0.0f);

    return {
        static_cast<float>(vec[0]), static_cast<float>(vec[1]), static_cast<float>(vec[2])
    };
}

inline glm::vec3 ConvertColor(const std::vector<double>& color)
{
    if (color.size() < 3)
        return glm::vec3(1.0f);

    return {static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2])};
}

namespace Rigel::Backend
{
    GLTF_Loader::GLTF_Loader() = default;
    GLTF_Loader::~GLTF_Loader() = default;

    bool GLTF_Loader::LoadModel(const std::filesystem::path& path, std::shared_ptr<ModelNode>& rootNode,
        std::vector<AssetHandle<Material>>& materials, std::vector<Vulkan::Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices)
    {
        m_Path = path;

        const auto extension = m_Path.extension();
        bool result = false;

        if (extension == ".gltf")
            result = m_Loader.LoadASCIIFromFile(&m_Model, &m_LoadError, &m_LoadWarning, m_Path.string());
        else if (extension == ".glb")
        {
            m_LoadError = ".glb format isn't supported yet!";
            return false;
        }
        else
        {
            m_LoadError = "Unsupported model format!";
            return false;
        }

        if (!result)
            return result;

        for (int32_t i = 0; i < m_Model.materials.size(); i++)
            materials.emplace_back(ProcessMaterial(i));

        // Create implicit root node because gltf doesn't have the concept of single root node per scene
        rootNode->Name = "RootNode";
        rootNode->Transform = glm::mat4(1.0f);

        const auto& scene = m_Model.scenes[m_Model.defaultScene >= 0 ? m_Model.defaultScene : 0];
        for (const auto nodeIdx : scene.nodes)
            ProcessNode(nodeIdx, rootNode, vertices, indices, materials);

        return result;
    }

    void GLTF_Loader::ProcessNode(const int nodeIdx, const std::shared_ptr<ModelNode>& curNode,
        std::vector<Vulkan::Vertex3p2t3n4g>& vertices, std::vector<uint32_t>& indices, std::vector<AssetHandle<Material>>& materials)
    {
        const auto& node = m_Model.nodes[nodeIdx];

        auto childNode = std::make_shared<ModelNode>();
        childNode->Name = node.name;
        childNode->Parent = curNode;

        if (node.matrix.size() == 16)
        {
            childNode->Transform = ConvertMat4(node.matrix);
        }
        else
        {
            glm::mat4 transform = glm::mat4(1.0f);

            if (node.translation.size() == 3)
                transform = glm::translate(transform, ConvertVec3(node.translation));

            if (node.rotation.size() == 4)
            {
                glm::quat rotation(
                    static_cast<float>(node.rotation[3]),
                    static_cast<float>(node.rotation[0]),
                    static_cast<float>(node.rotation[1]),
                    static_cast<float>(node.rotation[2])
                );
                transform *= glm::mat4_cast(rotation);
            }

            if (node.scale.size() == 3)
                transform = glm::scale(transform, ConvertVec3(node.scale));

            childNode->Transform = transform;
        }

        if (node.mesh >= 0)
        {
            const auto& mesh = m_Model.meshes[node.mesh];
            childNode->Meshes.reserve(mesh.primitives.size());

            for (const auto& primitive : mesh.primitives)
                childNode->Meshes.emplace_back(ProcessMesh(primitive, vertices, indices, materials));
        }

        for (const auto childIdx : node.children)
            ProcessNode(childIdx, childNode, vertices, indices, materials);

        curNode->Children.push_back(childNode);
    }

    ModelMesh GLTF_Loader::ProcessMesh(const tinygltf::Primitive& primitive, std::vector<Vulkan::Vertex3p2t3n4g>& vertices,
        std::vector<uint32_t>& indices, std::vector<AssetHandle<Material>>& materials)
    {
        auto resMesh = ModelMesh();
        resMesh.FirstVertex = vertices.size();
        resMesh.FirstIndex = indices.size();

        // Get positions
        const auto posIt = primitive.attributes.find("POSITION");
        if (posIt == primitive.attributes.end())
            return resMesh;

        const auto& posAccessor = m_Model.accessors[posIt->second];
        const auto& posBufferView = m_Model.bufferViews[posAccessor.bufferView];
        const auto& posBuffer = m_Model.buffers[posBufferView.buffer];
        const auto numVertices = posAccessor.count;

        // Get optional attributes
        const auto texIt = primitive.attributes.find("TEXCOORD_0");
        const auto normIt = primitive.attributes.find("NORMAL");
        const auto tanIt = primitive.attributes.find("TANGENT");

        const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);
        const float* texCoords = nullptr;
        const float* normals = nullptr;
        const float* tangents = nullptr;

        if (texIt != primitive.attributes.end())
        {
            const auto& texAccessor = m_Model.accessors[texIt->second];
            const auto& texBufferView = m_Model.bufferViews[texAccessor.bufferView];
            const auto& texBuffer = m_Model.buffers[texBufferView.buffer];
            texCoords = reinterpret_cast<const float*>(&texBuffer.data[texBufferView.byteOffset + texAccessor.byteOffset]);
        }

        if (normIt != primitive.attributes.end())
        {
            const auto& normAccessor = m_Model.accessors[normIt->second];
            const auto& normBufferView = m_Model.bufferViews[normAccessor.bufferView];
            const auto& normBuffer = m_Model.buffers[normBufferView.buffer];
            normals = reinterpret_cast<const float*>(&normBuffer.data[normBufferView.byteOffset + normAccessor.byteOffset]);
        }

        if (tanIt != primitive.attributes.end())
        {
            const auto& tanAccessor = m_Model.accessors[tanIt->second];
            const auto& tanBufferView = m_Model.bufferViews[tanAccessor.bufferView];
            const auto& tanBuffer = m_Model.buffers[tanBufferView.buffer];
            tangents = reinterpret_cast<const float*>(&tanBuffer.data[tanBufferView.byteOffset + tanAccessor.byteOffset]);
        }

        vertices.reserve(vertices.size() + numVertices);
        Vulkan::Vertex3p2t3n4g vertex {};

        for (uint32_t i = 0; i < numVertices; i++)
        {
            vertex.Position = {positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]};

            if (texCoords)
                vertex.TexCoords = {texCoords[i * 2], 1 - texCoords[i * 2 + 1]}; // Flip UVs!

            if (normals)
                vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};

            if (tangents)
                vertex.Tangent = {tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2], tangents[i * 4 + 3]};

            vertices.push_back(vertex);
        }

        resMesh.VertexCount = numVertices;

        // Process indices
        if (primitive.indices >= 0)
        {
            const auto& indexAccessor = m_Model.accessors[primitive.indices];
            const auto& indexBufferView = m_Model.bufferViews[indexAccessor.bufferView];
            const auto& indexBuffer = m_Model.buffers[indexBufferView.buffer];
            const auto numIndices = indexAccessor.count;

            indices.reserve(indices.size() + numIndices);

            const auto* data = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                const auto* buf = reinterpret_cast<const uint16_t*>(data);
                for (uint32_t i = 0; i < numIndices; ++i)
                    indices.push_back(static_cast<uint32_t>(buf[i]));
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                const auto* buf = reinterpret_cast<const uint32_t*>(data);
                for (uint32_t i = 0; i < numIndices; ++i)
                    indices.push_back(buf[i]);
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
            {
                const auto* buf = reinterpret_cast<const uint8_t*>(data);
                for (uint32_t i = 0; i < numIndices; ++i)
                    indices.push_back(static_cast<uint32_t>(buf[i]));
            }

            resMesh.IndexCount = numIndices;
        }

        if (primitive.material >= 0)
        {
            const auto& material = materials[primitive.material];
            material->WaitReady();
            resMesh.Material = material;
        }

        return resMesh;
    }

    std::filesystem::path GLTF_Loader::ProcessTexture(const tinygltf::Image& image, const std::filesystem::path& texturesDir)
    {
        if (!image.uri.empty())
            return texturesDir / image.uri;
        else
        {
            Debug::Error(".glb embedded textures aren't supported yet!");
            return {};
        }
    }

    AssetHandle<Material> GLTF_Loader::ProcessMaterial(const int materialIdx)
    {
        const auto materialName = m_Path / std::format("Material{}", materialIdx);
        const auto texturesDir = m_Path.parent_path();

        const auto& gltfMaterial = m_Model.materials[materialIdx];
        const auto& pbr = gltfMaterial.pbrMetallicRoughness;

        auto metadata = MaterialMetadata();

        // Albedo
        if (pbr.baseColorTexture.index >= 0)
        {
            const auto& texture = m_Model.textures[pbr.baseColorTexture.index];
            if (texture.source >= 0)
            {
                const auto& image = m_Model.images[texture.source];
                metadata.AlbedoTex = ProcessTexture(image, texturesDir);
            }
        }
        else
        {
            metadata.Color = ConvertColor(pbr.baseColorFactor);
        }

        // Metallic
        if (pbr.metallicRoughnessTexture.index >= 0)
        {
            const auto& texture = m_Model.textures[pbr.metallicRoughnessTexture.index];
            if (texture.source >= 0)
            {
                const auto& image = m_Model.images[texture.source];
                metadata.MetallicTex = ProcessTexture(image, texturesDir);
            }
        }
        else
        {
            metadata.Metalness = static_cast<float>(pbr.metallicFactor);
        }

        // Roughness
        if (pbr.metallicRoughnessTexture.index >= 0)
        {
            const auto& texture = m_Model.textures[pbr.metallicRoughnessTexture.index];
            if (texture.source >= 0)
            {
                const auto& image = m_Model.images[texture.source];
                metadata.RoughnessTex = ProcessTexture(image, texturesDir);
            }
        }
        else
        {
            metadata.Roughness = static_cast<float>(pbr.roughnessFactor);
        }

        // Normal
        if (gltfMaterial.normalTexture.index >= 0)
        {
            const auto& texture = m_Model.textures[gltfMaterial.normalTexture.index];
            if (texture.source >= 0)
            {
                const auto& image = m_Model.images[texture.source];
                metadata.NormalTex = ProcessTexture(image, texturesDir);
            }
        }

        // Ambient occlusion
        if (gltfMaterial.occlusionTexture.index >= 0)
        {
            const auto& texture = m_Model.textures[gltfMaterial.occlusionTexture.index];
            if (texture.source >= 0)
            {
                const auto& image = m_Model.images[texture.source];
                metadata.AmbientOcclusionTex = ProcessTexture(image, texturesDir);
            }
        }

        metadata.TwoSided = gltfMaterial.doubleSided;
        metadata.HasTransparency = gltfMaterial.alphaMode == "MASK" || gltfMaterial.alphaMode == "BLEND";

        return GetAssetManager()->LoadAsync<Material>(materialName, &metadata);
    }
}
