#pragma once

#include "Core.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

namespace rge
{
    class ModelRenderer : public Component
    {
    public:
        RGE_REGISTER_COMPONENT(ModelRenderer);

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    private:
        ModelRenderer();

        friend class GameObject;
    };
}
