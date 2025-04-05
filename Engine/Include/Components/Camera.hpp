#pragma once


#include "Core.hpp"
#include "Math.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

namespace rge
{
    class Camera final : public Component
    {
    public:
        RGE_REGISTER_COMPONENT(Camera);

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    INTERNAL:
        void RecreateFrustum(const glm::uvec2 viewport, const float32_t fov, const float32_t nearPlane, const float32_t farPlane);
    private:
        Camera();
        Camera(const float32_t fov, const float32_t nearPlane, const float32_t farPlane);
        ~Camera() override = default;

        void OnStart() override;

        glm::uvec2 m_ViewportSize;
        float32_t m_FOV;
        float32_t m_Near;
        float32_t m_Far;

        glm::mat4 m_Projection;
        glm::mat4 m_View;

        friend class GameObject;
    };
}
