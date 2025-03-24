#pragma once

namespace Input {
    class InputManager;
}

class Movement {
public:
    Movement(float speed, float sensitivity);

    void GetInput();

    void Move(float dt);

    void Orbit(float dt);

    void ImGuiDraw();

    const vec3 &GetPosition() { return m_Position; }
    const vec3 &GetForward() { return m_Forward; }

    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

    void SetLookSensitivity(float sensitivity) {
        m_LookSensitivity = sensitivity;
    }

    void SetOrbit(const vec3 &center, const float radius, const float speed, const float heightOffset) {
        m_OrbitCenter = center;
        m_OrbitRadius = radius;
        m_OrbitSpeed = speed;
        m_OrbitHeightOffset = heightOffset;
    }

private:
    vec3 m_MovementInput = {};
    vec2 m_LookInput = {};

    vec3 m_Position = {};
    vec3 m_Forward = {};
    vec3 m_Right = {};

    float m_MoveSpeed = 0.0f;
    float m_LookSensitivity = 0.0f;

    vec3 m_OrbitCenter = {};
    float m_OrbitRadius = 0.0f;
    float m_OrbitSpeed = 1.0f;
    float m_OrbitHeightOffset = 0.0f;

    Input::InputManager &m_InputManager;
};
