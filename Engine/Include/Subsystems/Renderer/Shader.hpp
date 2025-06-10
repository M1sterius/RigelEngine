#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Shader;
    }

    class Shader final : public RigelAsset
    {
    public:
        ~Shader() override;
    INTERNAL:
        NODISCARD Backend::Vulkan::VK_Shader& GetBackend() const { return *m_BackendShader; }
    private:
        explicit Shader(const std::filesystem::path& path);

        std::unique_ptr<Backend::Vulkan::VK_Shader> m_BackendShader;

        friend class AssetManager;
    };
}
