#pragma once

#include "RigelEngine.hpp"

class TestComponent final : public Rigel::Component
{
public:
    RGE_REGISTER_COMPONENT(TestComponent);

    NODISCARD nlohmann::json Serialize() const override;
    bool Deserialize(const nlohmann::json& json) override;
private:
    TestComponent() = default;
    ~TestComponent() override = default;

    void OnLoad() override;
    void OnStart() override;
    void OnDestroy() override;

    void OnGameUpdate(const Rigel::GameUpdateEvent& event);

    friend class Rigel::GameObject;
};

