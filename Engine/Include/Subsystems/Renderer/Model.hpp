#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel::Backend
{
    class BackendModel;
}

namespace Rigel
{
    class Model final : public RigelAsset
    {
    public:
        ~Model() override;
    INTERNAL:
        template<typename T> requires std::is_base_of_v<Backend::BackendModel, T>
        NODISCARD T& GetBackendModel() const
        {
            return static_cast<T&>(*m_BackendModel);
        }
    private:
        explicit Model(const std::filesystem::path& path);

        std::unique_ptr<Backend::BackendModel> m_BackendModel;

        friend class AssetManager;
    };
}
