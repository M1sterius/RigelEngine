#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan/vulkan.h"

namespace Rigel::Backend::Vulkan
{
    /*
     * Vertex layout naming scheme:
     *
     * number - how many floats a particular attribute occupies
     * letter - what type of attribute it is
     *
     * p - position
     * t - texture coordinates
     * n - normal
     * g - tangent
     */

    struct Vertex3p2t3n4g
    {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
        glm::vec4 Tangent;

        Vertex3p2t3n4g() = default;
        Vertex3p2t3n4g(const glm::vec3& position, const glm::vec2& texCoords, const glm::vec4& normal, const glm::vec4& tangent)
            : Position(position), TexCoords(texCoords), Normal(normal), Tangent(tangent) { }

        NODISCARD static VkVertexInputBindingDescription GetBindingDescription()
        {
            /*
             * Apparently it specifies how many vertex buffers will be used inside the shader
             * Something like bind multiple VBOs to a single VAO in OpenGL
             * There we use only one vertex buffer in the shader so we have a single binding description
             */

            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex3p2t3n4g);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        NODISCARD static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions {};

            // a_Position (location = 0)
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex3p2t3n4g, Position);

            // a_TexCoords (location = 1)
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex3p2t3n4g, TexCoords);

            // a_Normal (location = 2)
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex3p2t3n4g, Normal);

            // a_Tangent(location = 3)
            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex3p2t3n4g, Tangent);

            return attributeDescriptions;
        }
    };

    struct Vertex3p2t
    {
        glm::vec3 Position;
        glm::vec2 TexCoords;

         Vertex3p2t() = default;
        Vertex3p2t(const glm::vec3 pos, const glm::vec2 texCoords)
            : Position(pos), TexCoords(texCoords) { }

        NODISCARD static VkVertexInputBindingDescription GetBindingDescription()
        {
            /*
             * Apparently it specifies how many vertex buffers will be used inside the shader
             * Something like bind multiple VBOs to a single VAO in OpenGL
             * There we use only one vertex buffer in the shader so we have a single binding description
             */

            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex3p2t);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        NODISCARD static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

            // a_Position (location = 0)
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex3p2t, Position);

            // a_TexCoords (location = 1)
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex3p2t, TexCoords);

            return attributeDescriptions;
        }
    };
}
