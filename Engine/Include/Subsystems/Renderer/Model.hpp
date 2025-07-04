#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Model;
    }

    class Model final : public RigelAsset
    {
    public:
        ~Model() override;
    INTERNAL:
        NODISCARD Backend::Vulkan::VK_Model& GetBackend() const { return *m_BackendModel; }
    private:
        Model(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        std::unique_ptr<Backend::Vulkan::VK_Model> m_BackendModel;

        friend class AssetManager;
    };
}
