#pragma once

#include "Core.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

#include "Model.hpp"
#include "AssetHandle.hpp"

#include <filesystem>

namespace rge
{
    class ModelRenderer final : public Component
    {
    public:
        RGE_REGISTER_COMPONENT(ModelRenderer);

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

        friend class GameObject;
    };
}
