#pragma once

#include "Render/Camera.hpp"
#include "Movement.hpp"

class FirstPersonCamera {
public:
    FirstPersonCamera(float speed,
                      float sensitivity,
                      float windowWidth,
                      float windowHeight);

    void Update(float dt);

    const mat4 &GetProjection() const { return m_Camera.GetProjection(); }

    void SetProjection(const float width, const float height) {
        m_Camera.Perspective(0.1f, 100.0f, width / height, 45.0f);
    }

    const mat4 &GetView() const { return m_Camera.GetView(); }

    const mat4 &GetProjView() const { return m_Camera.GetProjView(); }

    const mat4 &GetInvProjection() const { return m_Camera.GetInvProjection(); }
    const mat4 &GetInvView() const { return m_Camera.GetInvView(); }

    const vec3 &GetPosition() { return m_Movement.GetPosition(); }
    const vec3 &GetForward() { return m_Movement.GetForward(); }

    Camera *GetCamera() { return &m_Camera; }

    Movement &GetMovement() { return m_Movement; }

private:
    Camera m_Camera;
    Movement m_Movement;
};
