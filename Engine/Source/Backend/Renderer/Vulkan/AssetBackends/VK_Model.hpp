#pragma once

#include "../BackendModel.hpp"

namespace rge::backend
{
    class VK_Model final : public BackendModel
    {
    public:
        explicit VK_Model(const std::filesystem::path& path);
        ~VK_Model() override;
    };
}
