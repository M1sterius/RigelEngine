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

    void OnLoad() override;
    void OnStart() override;
    void OnDestroy() override;

    Rigel::ComponentHandle<Rigel::ModelRenderer> m_ModelRenderer;

    void OnGameUpdate(const Rigel::GameUpdateEvent& event);
};

