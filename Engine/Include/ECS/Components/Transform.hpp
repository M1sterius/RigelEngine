#pragma once

#include "Utils/HeaderUtils/HeaderUtils.hpp"
#include "../Component.hpp"
#include "ECS/GameObject.hpp"

namespace rge
{
    class Transform final : public Component
    {
    public:
        ~Transform() override = default;
    private:
        Transform();

        NODISCARD nlohmann::json Serialize() const override;
        void Deserialize(const nlohmann::json& json) override;

        friend class GameObject;
    };
}
