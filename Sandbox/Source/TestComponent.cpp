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
    rge::Debug::Message("OnLoad");
}

void TestComponent::OnStart()
{
    rge::Debug::Message("OnStart");
    EventManager.Subscribe<rge::GameUpdateEvent>(
        [this](const rge::Event& e) { this->OnUpdate(e); });
}

void TestComponent::OnDestroy()
{

}

void TestComponent::OnUpdate(const rge::Event& e)
{
    const auto& event = rge::CastEvent<rge::GameUpdateEvent>(e);
    rge::Debug::Message(std::to_string(event.frameIndex));
}
