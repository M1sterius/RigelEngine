#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace rge::backend
{
    class BackendModel;
}

namespace rge
{
    class Model final : public RigelAsset
    {
    public:
        ~Model() override;
    INTERNAL:
        template<typename T> requires std::is_base_of_v<backend::BackendModel, T>
        NODISCARD T& GetBackendModel() const
        {
            return static_cast<T&>(*m_BackendModel);
        }
    private:
        explicit Model(const std::filesystem::path& path);

        std::unique_ptr<backend::BackendModel> m_BackendModel;

        friend class AssetManager;
    };
}
