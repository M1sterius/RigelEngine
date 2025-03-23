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
        ~Shader() override;
    INTERNAL:
        template<typename T>
        NODISCARD T& GetBackendShader() const
        {
            return static_cast<T&>(*m_BackendShader);
        }
    private:
        explicit Shader(const std::filesystem::path& path);

        std::unique_ptr<backend::BackendShader> m_BackendShader;

        friend class AssetManager;
    };
}
