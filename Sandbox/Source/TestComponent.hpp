#pragma once

#include "RigelEngine.hpp"

class TestComponent final : public Rigel::Component
{
public:
    RIGEL_REGISTER_COMPONENT(TestComponent);

    NODISCARD nlohmann::json Serialize() const override;
    bool Deserialize(const nlohmann::json& json) override;
private:
    TestComponent() = default;
    ~TestComponent() override = default;

    void OnStart() override;
    void OnDestroy() override;

    void OnGameUpdate();
    void OnPhysicsTick();

    Rigel::ComponentHandle<Rigel::ModelRenderer> m_ModelRenderer;
    uid_t m_UpdateEventCallbackID = NULL_ID;
};

