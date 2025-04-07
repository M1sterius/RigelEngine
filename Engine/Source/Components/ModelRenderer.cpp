#include "ModelRenderer.hpp"
#include "json.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace rge
{
    ModelRenderer::ModelRenderer()
        : m_Model(AssetHandle<Model>::Null()) { }

    ModelRenderer::ModelRenderer(AssetHandle<Model> model)
        : m_Model(std::move(model)) { }

    ModelRenderer::ModelRenderer(const std::filesystem::path& modelPath)
        : m_ModelPath(modelPath) { }

    void ModelRenderer::OnLoad()
    {
        if (m_Model.IsNull() && m_ModelPath.has_value())
        {
            auto& assetManager = Engine::Get().GetAssetManager();
            m_Model = assetManager.Load<Model>(m_ModelPath.value());
        }
    }

    nlohmann::json ModelRenderer::Serialize() const
    {
        return Component::Serialize();
    }

    bool ModelRenderer::Deserialize(const nlohmann::json& json)
    {
        return Component::Deserialize(json);
    }
}
