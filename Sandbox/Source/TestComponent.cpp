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
    EventManager.Subscribe<Rigel::GameUpdateEvent>(
    [this](const Rigel::GameUpdateEvent& e) { this->OnGameUpdate(e); }
    );
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
}
