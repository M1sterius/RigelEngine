#pragma once

#include "RigelEngine.hpp"

class FreeCamController : public Rigel::Component
{
public:
    RIGEL_REGISTER_COMPONENT(FreeCamController);

    NODISCARD nlohmann::json Serialize() const override;
    bool Deserialize(const nlohmann::json& json) override;

    float32_t Sensitivity = 0.3f;
    float32_t Speed = 2.0f;
private:
    FreeCamController() = default;
    ~FreeCamController() override = default;

    void OnStart() override;
    void OnUpdate();

    float32_t m_Yaw = 0;
    float32_t m_Pitch = 0;
};
