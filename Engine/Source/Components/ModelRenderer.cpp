#include "ModelRenderer.hpp"
#include "json.hpp"

namespace rge
{
    nlohmann::json ModelRenderer::Serialize() const
    {
        return Component::Serialize();
    }

    bool ModelRenderer::Deserialize(const nlohmann::json& json)
    {
        return Component::Deserialize(json);
    }

    ModelRenderer::ModelRenderer()
    {

    }
}
