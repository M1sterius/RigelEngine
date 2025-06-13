#include "ModelRenderer.hpp"
#include "json.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

namespace Rigel
{
    ModelRenderer::ModelRenderer()
        : m_Model(AssetHandle<Model>::Null()) { }

    ModelRenderer::ModelRenderer(AssetHandle<Model> model)
        : m_Model(std::move(model)) { }

    ModelRenderer::ModelRenderer(const std::filesystem::path& modelPath)
        : m_ModelPath(modelPath) { }

    AssetHandle<Model> ModelRenderer::GetModelAsset() const
    {
        if (!m_Model.IsNull())
            m_Model->WaitReady();
        return m_Model;
    }

    void ModelRenderer::OnLoad()
    {
        if (m_Model.IsNull() && m_ModelPath.has_value())
        {
            auto& assetManager = Engine::Get().GetAssetManager();
            m_Model = assetManager.LoadAsync<Model>(m_ModelPath.value());
        }
    }

    nlohmann::json ModelRenderer::Serialize() const
    {
        auto json = Component::Serialize();
        const auto& assetManager = Engine::Get().GetAssetManager();

        if (m_ModelPath.has_value())
            json["ModelPath"] = m_ModelPath;
        else if (!m_Model.IsNull())
            json["ModelPath"] = assetManager.GetAssetPath(m_Model);
        else
        {
            json["ModelPath"] = "";
            Debug::Error("Failed to serialize Rigel::ModelRenderer component. No model asset specified!");
        }

        return json;
    }

    bool ModelRenderer::Deserialize(const nlohmann::json& json)
    {
        Component::Deserialize(json);

        if (json["Type"] != GetTypeName())
        {
            Debug::Error("Failed to deserialize Rigel::ModelRenderer component. Type mismatch!");
            return false;
        }

        if (!json.contains("ModelPath"))
        {
            Debug::Error("Failed to deserialize Rigel::ModelRenderer! Some of the required data is not present in the json object.");
            return false;
        }

        m_ModelPath = json["ModelPath"].get<std::filesystem::path>();

        return true;
    }
}
