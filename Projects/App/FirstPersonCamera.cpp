#include "FirstPersonCamera.hpp"

FirstPersonCamera::FirstPersonCamera(float speed,
                                     float sensitivity,
                                     float windowWidth,
                                     float windowHeight)
    : m_Movement(Movement(speed, sensitivity)) {
    m_Camera.Perspective(0.1f, 100.0f, windowWidth / windowHeight, 45.0f);
    m_Camera.LookAt(vec3(0.0f), vec3(0.0f, 1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
}

void
FirstPersonCamera::Update(float dt) {
    m_Movement.Move(dt);

    m_Camera.LookAt(m_Movement.GetPosition(),
                    m_Movement.GetPosition() + m_Movement.GetForward(),
                    vec3(0.0f, 1.0f, 0.0f));
}
