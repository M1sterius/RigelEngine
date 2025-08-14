#pragma once

#include "Core.hpp"
#include "ECS/Component.hpp"
#include "Assets/Model.hpp"
#include "Handles/AssetHandle.hpp"

#include <filesystem>

namespace Rigel
{
    class ModelRenderer final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::ModelRenderer);

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD AssetHandle<Model> GetModelAsset() const;
    private:
        ModelRenderer();
        explicit ModelRenderer(const AssetHandle<Model>& model);
        explicit ModelRenderer(const std::filesystem::path& modelPath);

        void OnLoad() override;

        AssetHandle<Model> m_Model;
        std::optional<std::filesystem::path> m_ModelPath;
    };
}
