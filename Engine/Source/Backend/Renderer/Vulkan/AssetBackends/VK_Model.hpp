#pragma once

#include "Core.hpp"
#include "../BackendModel.hpp"

#include <memory>
#include <string_view>

namespace rge::backend
{
    class VK_VertexBuffer;
    class VK_IndexBuffer;

    class VK_Model final : public BackendModel
    {
    public:
        explicit VK_Model(const std::filesystem::path& path);
        ~VK_Model() override;
    private:
        std::unique_ptr<VK_VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VK_IndexBuffer> m_IndexBuffer;

        NODISCARD static bool IsFormatSupported(const std::string_view& extension);
    };
}
