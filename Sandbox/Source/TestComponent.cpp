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
    EventManager.Subscribe<rge::GameUpdateEvent>(
    [this](const rge::GameUpdateEvent& e) { this->OnGameUpdate(e); }
    );
}

void TestComponent::OnDestroy()
{

}

void TestComponent::OnGameUpdate(const rge::GameUpdateEvent& event)
{
    if (rge::Input::GetKeyDown(rge::KeyCode::SPACE))
        rge::Debug::Message("Space is pressed!");
    if (rge::Input::GetKeyUp(rge::KeyCode::SPACE))
        rge::Debug::Message("Space is released!");
}
