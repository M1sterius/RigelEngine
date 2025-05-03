#pragma once

#include "Core.hpp"
#include "Component.hpp"

#include "Model.hpp"
#include "AssetHandle.hpp"

#include <filesystem>

namespace Rigel
{
    class ModelRenderer final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::ModelRenderer);

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD AssetHandle<Model> GetModel() const { return m_Model; }
    private:
        ModelRenderer();
        explicit ModelRenderer(AssetHandle<Model> model);
        explicit ModelRenderer(const std::filesystem::path& modelPath);

        void OnLoad() override;

        AssetHandle<Model> m_Model;
        std::optional<std::filesystem::path> m_ModelPath;
    };
}
