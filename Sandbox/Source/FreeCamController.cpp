#include "FreeCamController.hpp"

#include <numbers>
#define HALF_PI (std::numbers::pi / 2)

nlohmann::json FreeCamController::Serialize() const
{
    auto json = Component::Serialize();

    json["Sensitivity"] = Sensitivity;
    json["Speed"] = Speed;

    return json;
}

bool FreeCamController::Deserialize(const nlohmann::json& json)
{
    Component::Deserialize(json);

    if (json["Type"] != GetTypeName())
    {
        Rigel::Debug::Error("Failed to deserialize FreeCamController component. Type mismatch!");
        return false;
    }

    if (!json.contains("Sensitivity") || !json.contains("Speed"))
    {
        Rigel::Debug::Error("Failed to deserialize FreeCamController. Some of the required data is missing!");
        return false;
    }

    Sensitivity = json["Sensitivity"].get<float32_t>();
    Speed = json["Speed"].get<float32_t>();

    return true;
}

void FreeCamController::OnStart()
{
    SubscribeEvent<Rigel::GameUpdateEvent>(OnUpdate);
}

void FreeCamController::OnUpdate()
{
    auto transform = this->GetGameObject()->GetTransform();
    auto windowManager = Rigel::GetWindowManager();
    auto inputManager = Rigel::GetInputManager();

    if (inputManager->GetKeyDown(Rigel::KeyCode::F9))
        windowManager->SetDisplayMode(Rigel::DisplayMode::Windowed);
    else if (inputManager->GetKeyDown(Rigel::KeyCode::F10))
        windowManager->SetDisplayMode(Rigel::DisplayMode::WindowedBorderless);
    else if (inputManager->GetKeyDown(Rigel::KeyCode::F11))
        windowManager->SetDisplayMode(Rigel::DisplayMode::Fullscreen);

    if (inputManager->GetKeyDown(Rigel::KeyCode::ESCAPE))
    {
        if (windowManager->GetCursorState() == Rigel::CursorState::Normal)
            windowManager->SetCursorState(Rigel::CursorState::Disabled);
        else if (windowManager->GetCursorState() == Rigel::CursorState::Disabled)
            windowManager->SetCursorState(Rigel::CursorState::Normal);
    }

    if (windowManager->GetCursorState() == Rigel::CursorState::Disabled)
    {
        const auto mouseDelta = inputManager->GetMouseDelta();
        m_Yaw -= mouseDelta.x * Sensitivity * Rigel::Time::GetDeltaTimeF();
        m_Pitch -= mouseDelta.y * Sensitivity * Rigel::Time::GetDeltaTimeF();
    }

    if (m_Pitch > HALF_PI) m_Pitch = HALF_PI;
    else if (m_Pitch < -HALF_PI) m_Pitch = -HALF_PI;

    const auto dt = Rigel::Time::GetDeltaTimeF();
    const auto rv = transform->GetRightVector();
    const auto fv = transform->GetForwardVector();
    auto pos = transform->GetLocalPosition();

    glm::vec3 dir(0.0f);

    if (inputManager->GetKeyPressed(Rigel::KeyCode::W))
        dir += fv;
    if (inputManager->GetKeyPressed(Rigel::KeyCode::S))
        dir -= fv;
    if (inputManager->GetKeyPressed(Rigel::KeyCode::A))
        dir -= rv;
    if (inputManager->GetKeyPressed(Rigel::KeyCode::D))
        dir += rv;

    if (glm::length(dir) > 0.001f)
        pos += glm::normalize(dir) * Speed * dt;

    if (inputManager->GetKeyPressed(Rigel::KeyCode::Q))
        pos += Rigel::Transform::WORLD_UP * Speed * dt;
    if (inputManager->GetKeyPressed(Rigel::KeyCode::E))
        pos -= Rigel::Transform::WORLD_UP * Speed * dt;

    transform->SetLocalPosition(pos);
    transform->SetLocalRotation(glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f)));
}
