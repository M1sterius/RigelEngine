#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan.h"

#include <vector>
#include <memory>

namespace rge::backend
{
    class VK_Device;
    class VK_MemoryBuffer;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoords;

        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec2& texCoords)
            :   position(position), texCoords(texCoords) { }

        NODISCARD inline static VkVertexInputBindingDescription GetBindingDescription()
        {
            /*
             * Apparently it specifies how many vertex buffers will be used inside the shader
             * Something like bind multiple VBOs to a single VAO in OpenGL
             * There we use only one vertex buffer in the shader so we have a single binding description
             */

            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        NODISCARD inline static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

            // a_Position (location = 0)
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            // a_TexCoords (location = 1)
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoords);

            return attributeDescriptions;
        }
    };

    class VK_VertexBuffer 
    {
    public:
        VK_VertexBuffer(VK_Device& device, const std::vector<Vertex>& vertices);
        ~VK_VertexBuffer();

        VK_VertexBuffer(const VK_VertexBuffer&) = delete;
        VK_VertexBuffer operator = (const VK_VertexBuffer&) = delete;
    private:
        std::unique_ptr<VK_MemoryBuffer> m_MemoryBuffer;
        uint32_t m_VertexCount;
    };
}
