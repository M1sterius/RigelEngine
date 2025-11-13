#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ECS/Component.hpp"

namespace Rigel
{
    class DirectionalLight final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::DirectionalLight);

        glm::vec3 Direction = {0.0, 0.0, -1.0};
        glm::vec3 Color{1.0};
        float32_t Intensity{1.0};

        bool CastShadows{true};

        NODISCARD nlohmann::json Serialize() const override
        {
            auto json = Component::Serialize();
            return json;
        }

        bool Deserialize(const nlohmann::json& json) override
        {
            Component::Deserialize(json);
            return true;
        }
    private:
        DirectionalLight() = default;
        ~DirectionalLight() override = default;
    };
}