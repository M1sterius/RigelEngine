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

}

void TestComponent::OnDestroy()
{

}

