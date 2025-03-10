#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace rge::backend
{
    class BackendShader;
}

namespace rge
{
    class Shader final : public RigelAsset
    {
    public:
        Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);
        ~Shader() override;
    private:
        std::unique_ptr<backend::BackendShader> m_Backend;
    };
}