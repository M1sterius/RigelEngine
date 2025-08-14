#include "Camera.hpp"
#include "json.hpp"
#include "GameObject.hpp"
#include "Engine.hpp"
#include "WindowManager.hpp"
#include "EventManager.hpp"
#include "SubsystemGetters.hpp"

namespace Rigel
{
    Camera::Camera()
        : m_ViewportSize(GetWindowManager()->GetWindowSize()), m_FOV(3.141592 / 3), m_Near(0.1), m_Far(100.0)
    {
        CalcProjection();
    }

    Camera::Camera(const float32_t fov, const float32_t nearPlane, const float32_t farPlane)
        : m_ViewportSize(GetWindowManager()->GetWindowSize()), m_FOV(fov), m_Near(nearPlane), m_Far(farPlane)
    {
        CalcProjection();
    }

    glm::mat4 Camera::GetView()
    {
        auto transform = GetGameObject()->GetTransform();

        const auto pos = transform->GetPosition();
        const auto rot = transform->GetRotation();
        const auto transformation = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(rot);
        m_View = glm::inverse(transformation);

        return m_View;
    }

    glm::mat4 Camera::GetProjection()
    {
        return m_Projection;
    }

    void Camera::SetFov(const float32_t fov)
    {
        m_FOV = fov;
        CalcProjection();
    }

    void Camera::SetPlanes(const float32_t nearPlane, const float32_t farPlane)
    {
        m_Near = nearPlane;
        m_Far = farPlane;
        CalcProjection();
    }

    void Camera::OnStart()
    {
        SubscribeEvent<WindowResizeEvent>(&Camera::OnWindowResize);
    }

    void Camera::OnWindowResize()
    {
        m_ViewportSize = GetWindowManager()->GetWindowSize();
        CalcProjection();
    }

    void Camera::CalcProjection()
    {
        if (m_ViewportSize.x == 0 || m_ViewportSize.y == 0) return;

        const auto aspect = static_cast<float32_t>(m_ViewportSize.x) / static_cast<float32_t>(m_ViewportSize.y);
        m_Projection = glm::perspective(m_FOV * aspect, aspect, m_Near, m_Far);
        m_Projection[1][1] *= -1.0f;
    }

    nlohmann::json Camera::Serialize() const
    {
        auto json = Component::Serialize();

        json["FOV"] = m_FOV;
        json["Near"] = m_Near;
        json["Far"] = m_Far;

        return json;
    }

    bool Camera::Deserialize(const nlohmann::json& json)
    {
        Component::Deserialize(json);

        if (json["Type"] != GetTypeName())
        {
            Debug::Error("Failed to deserialize Rigel::Camera component. Type mismatch!");
            return false;
        }

        if (!json.contains("FOV") || !json.contains("Near") || !json.contains("Far"))
        {
            Debug::Error("Failed to deserialize Rigel::Camera. Some of the required data is missing!");
            return false;
        }

        m_FOV = json["FOV"].get<float32_t>();
        m_Near = json["Near"].get<float32_t>();
        m_Far = json["Far"].get<float32_t>();
        CalcProjection();

        return true;
    }
}
