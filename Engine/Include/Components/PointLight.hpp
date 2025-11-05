#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ECS/Component.hpp"

namespace Rigel
{
    class PointLight final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::PointLight);

        float32_t Constant{1.0};
        float32_t Linear{0.35};
        float32_t Quadratic{0.44};

        glm::vec3 Color{1.0};
        float32_t IntensityP{1.0};

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
        PointLight() = default;
        ~PointLight() override = default;
    };
}