#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "ECS/Component.hpp"

namespace Rigel
{
    class Camera final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::Camera);

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD glm::mat4 GetView();
        NODISCARD glm::mat4 GetProjection();

        void SetFov(const float32_t fov);
        void SetPlanes(const float32_t nearPlane, const float32_t farPlane);
    private:
        Camera();
        Camera(const float32_t fov, const float32_t nearPlane, const float32_t farPlane);
        ~Camera() override = default;

        void OnStart() override;

        void OnWindowResize();
        void CalcProjection();

        glm::uvec2 m_ViewportSize;
        float32_t m_FOV;
        float32_t m_Near;
        float32_t m_Far;

        glm::mat4 m_Projection{};
        glm::mat4 m_View{};
    };
}
