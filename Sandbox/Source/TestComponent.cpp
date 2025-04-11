#include "TestComponent.hpp"

nlohmann::json TestComponent::Serialize() const
{
    return Component::Serialize();
}

bool TestComponent::Deserialize(const nlohmann::json& json)
{
    return Component::Deserialize(json);
}

void TestComponent::OnLoad()
{

}

void TestComponent::OnStart()
{
    GetEventManager().Subscribe<Rigel::GameUpdateEvent>(
    [this](const Rigel::GameUpdateEvent& e) { this->OnGameUpdate(e); }
    );

    m_ModelRenderer = this->GetGameObject()->GetComponent<Rigel::ModelRenderer>();
}

void TestComponent::OnDestroy()
{

}

void TestComponent::OnGameUpdate(const Rigel::GameUpdateEvent& event)
{
    if (Rigel::Input::GetKeyDown(Rigel::KeyCode::SPACE))
        Rigel::Debug::Message("Space is pressed!");
    if (Rigel::Input::GetKeyUp(Rigel::KeyCode::SPACE))
        Rigel::Debug::Message("Space is released!");

    constexpr glm::vec3 angle = {0, 1, 1};
    const auto rot = glm::quat(angle * Rigel::Time::GetGlobalTimeF());
    m_ModelRenderer->GetGameObject()->GetTransform()->SetRotation(rot);
}
