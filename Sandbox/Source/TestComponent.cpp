#include "TestComponent.hpp"

nlohmann::json TestComponent::Serialize() const
{
    return Component::Serialize();
}

bool TestComponent::Deserialize(const nlohmann::json& json)
{
    return Component::Deserialize(json);
}

void TestComponent::OnStart()
{
    SubscribeEvent<Rigel::GameUpdateEvent>(OnGameUpdate);
    SubscribeEvent<Rigel::PhysicsTickEvent>(OnPhysicsTick);
    m_ModelRenderer = this->GetGameObject()->GetComponent<Rigel::ModelRenderer>();
}

void TestComponent::OnDestroy()
{

}

void TestComponent::OnGameUpdate()
{
    if (Rigel::Input::GetKeyDown(Rigel::KeyCode::SPACE))
        this->SetActive(false);

    constexpr glm::vec3 angle = {0, 1, 1};
    const auto rot = glm::quat(angle * Rigel::Time::GetGlobalTimeF());
    m_ModelRenderer->GetGameObject()->GetTransform()->SetRotation(rot);
}

void TestComponent::OnPhysicsTick()
{
    if (Rigel::Time::GetGlobalTimeF() > 3.0f)
        this->SetActive(false);
}
