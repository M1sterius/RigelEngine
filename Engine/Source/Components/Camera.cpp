#include "Camera.hpp"
#include "json.hpp"

#include "Engine.hpp"
#include "WindowManager.hpp"

namespace Rigel
{
    Camera::Camera()
    {

    }

    Camera::Camera(const float32_t fov, const float32_t nearPlane, const float32_t farPlane)
    {

    }

    void Camera::RecreateFrustum(const glm::uvec2 viewport, const float32_t fov, const float32_t nearPlane,
        const float32_t farPlane)
    {

    }

    void Camera::OnStart()
    {

    }

    nlohmann::json Camera::Serialize() const
    {
        return Component::Serialize();
    }

    bool Camera::Deserialize(const nlohmann::json& json)
    {
        return Component::Deserialize(json);
    }
}
