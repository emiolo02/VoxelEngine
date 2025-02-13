#include "Movement.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"
#include "Input/InputManager.hpp"
#include "Input/Keyboard.hpp"
#include <algorithm>

Movement::Movement(float speed, float sensitivity)
  : m_InputManager(Input::InputManager::Get())
    , m_MoveSpeed(speed)
    , m_LookSensitivity(sensitivity) {
  m_Forward = vec3(0.0f, 0.0f, -1.0f);
  m_Position = vec3();
}

void
Movement::Move(float dt) {
  if (m_LookInput.y > 89.0f)
    m_LookInput.y = 89.0f;
  if (m_LookInput.y < -89.0f)
    m_LookInput.y = -89.0f;

  m_LookInput += m_InputManager.mouse.GetDeltaPosition() * vec2(1, -1) *
      m_LookSensitivity * dt;

  m_Forward.x = cos(radians(m_LookInput.x)) * cos(radians(m_LookInput.y));
  m_Forward.y = sin(radians(m_LookInput.y));
  m_Forward.z = sin(radians(m_LookInput.x)) * cos(radians(m_LookInput.y));

  m_Right = cross(m_Forward, vec3(0.0f, 1.0f, 0.0f));

  m_MovementInput = vec3();
  if (m_InputManager.keyboard.GetHeld(Input::Key::W))
    m_MovementInput.z = 1.0f;
  if (m_InputManager.keyboard.GetHeld(Input::Key::S))
    m_MovementInput.z = -1.0f;
  if (m_InputManager.keyboard.GetHeld(Input::Key::A))
    m_MovementInput.x = -1.0f;
  if (m_InputManager.keyboard.GetHeld(Input::Key::D))
    m_MovementInput.x = 1.0f;
  if (m_InputManager.keyboard.GetHeld(Input::Key::Q))
    m_MovementInput.y = -1.0f;
  if (m_InputManager.keyboard.GetHeld(Input::Key::E))
    m_MovementInput.y = 1.0f;

  vec3 velocity = m_MovementInput.z * m_Forward + m_MovementInput.x * m_Right +
                  m_MovementInput.y * vec3(0.0f, 1.0f, 0.0f);

  m_MoveSpeed += (float) m_InputManager.mouse.GetDeltaScroll() / 10.0f;
  m_MoveSpeed = std::max(m_MoveSpeed, 0.5f);

  m_Position += velocity * m_MoveSpeed * dt;
}

void
Movement::ImGuiDraw() {
  ImGui::Begin("Movement");
  ImGui::DragFloat3("Position", &m_Position[0]);
  ImGui::End();
}
