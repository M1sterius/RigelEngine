#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel::Backend
{
    class IShaderBackend;
}

namespace Rigel
{
    class Shader final : public RigelAsset
    {
    public:
        ~Shader() override;
    INTERNAL:
        template<typename T> requires std::is_base_of_v<Backend::IShaderBackend, T>
        NODISCARD T& GetBackend() const
        {
            return static_cast<T&>(*m_BackendShader);
        }
    private:
        explicit Shader(const std::filesystem::path& path);

        std::unique_ptr<Backend::IShaderBackend> m_BackendShader;

        friend class AssetManager;
    };
}
