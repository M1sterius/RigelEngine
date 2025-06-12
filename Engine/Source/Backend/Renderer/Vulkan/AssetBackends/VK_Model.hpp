#pragma once

#include "Core.hpp"

#include <memory>
#include <string_view>

namespace Rigel::Backend::Vulkan
{
    class VK_VertexBuffer;
    class VK_IndexBuffer;

    class VK_Model final
    {
    public:
        explicit VK_Model(std::filesystem::path path);
        ~VK_Model();

        NODISCARD inline VK_VertexBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
        NODISCARD inline VK_IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }
    private:
        std::unique_ptr<VK_VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VK_IndexBuffer> m_IndexBuffer;

        NODISCARD static bool IsFormatSupported(const std::string_view& extension);

        const std::filesystem::path m_Path;
    };
}
