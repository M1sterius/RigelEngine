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
    m_ModelRenderer = this->GetGameObject()->GetComponent<Rigel::ModelRenderer>();
}

void TestComponent::OnDestroy()
{

}

void TestComponent::OnGameUpdate()
{
    constexpr glm::vec3 angle = {0, 1, 0};
    const auto rot = glm::quat(angle * Rigel::Time::GetGlobalTimeF());
    m_ModelRenderer->GetGameObject()->GetTransform()->SetLocalRotation(rot);

    auto windowManager = Rigel::GetWindowManager();

    if (Rigel::Input::GetKeyDown(Rigel::KeyCode::F9))
        windowManager->SetDisplayMode(Rigel::DisplayMode::Windowed);
    else if (Rigel::Input::GetKeyDown(Rigel::KeyCode::F10))
        windowManager->SetDisplayMode(Rigel::DisplayMode::WindowedBorderless);
    else if (Rigel::Input::GetKeyDown(Rigel::KeyCode::F11))
        windowManager->SetDisplayMode(Rigel::DisplayMode::Fullscreen);
}

