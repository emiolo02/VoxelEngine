#pragma once

namespace Input {
class InputManager;
}

class Movement
{
public:
  Movement(float speed, float sensitivity);
  void GetInput();
  void Move(float dt);

  void ImGuiDraw();

  inline const vec3& GetPosition() { return m_Position; }
  inline const vec3& GetForward() { return m_Forward; }

  inline void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
  inline void SetLookSensitivity(float sensitivity)
  {
    m_LookSensitivity = sensitivity;
  }

private:
  vec3 m_MovementInput = vec3();
  vec2 m_LookInput = vec2();

  vec3 m_Position = vec3();
  vec3 m_Forward = vec3();
  vec3 m_Right = vec3();

  float m_MoveSpeed = 0.0f;
  float m_LookSensitivity = 0.0f;

  Input::InputManager& m_InputManager;
};
