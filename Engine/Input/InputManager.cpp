#include "InputManager.hpp"
#include "GLFW/glfw3.h"

namespace Input {
  void
  InputManager::BeginFrame() {
    for (int i = 0; i < Input::Key::NumKeyCodes; i++) {
      keyboard.m_Pressed[i] = false;
      keyboard.m_Released[i] = false;
    }
    for (int i = 0; i < Input::MouseButton::numButtons; i++) {
      mouse.m_Pressed[i] = false;
      mouse.m_Released[i] = false;
    }
    mouse.m_DeltaPosition = vec2();
    mouse.m_DeltaScroll = 0;
  }

  void
  InputManager::HandleKeyEvent(int32 key, int32 action) {
    if (action == GLFW_PRESS) {
      keyboard.m_Pressed[Input::Key::FromGLFW(key)] = true;
      keyboard.m_Held[Input::Key::FromGLFW(key)] = true;
    } else if (action == GLFW_RELEASE) {
      keyboard.m_Released[Input::Key::FromGLFW(key)] = true;
      keyboard.m_Held[Input::Key::FromGLFW(key)] = false;
    }
  }

  void
  InputManager::HandleMousePosition(float64 x, float64 y, int32 w, int32 h) {
    mouse.m_Position = vec2(x, y);
    mouse.m_NormalizedPosition = vec2(2.0f * x / w - 1.0f, 2.0f * y / h - 1.0f);

    mouse.m_DeltaPosition = mouse.m_Position - mouse.m_LastPosition;

    mouse.m_LastPosition = mouse.m_Position;
  }

  void
  InputManager::HandleMouseScroll(float64 x, float64 y) {
    mouse.m_DeltaScroll = (int8) y;
  }

  void
  InputManager::HandleMouseButton(int32 button, int32 action) {
    if (action == GLFW_PRESS) {
      mouse.m_Pressed[Input::MouseButton::FromGLFW(button)] = true;
      mouse.m_Held[Input::MouseButton::FromGLFW(button)] = true;
    } else if (action == GLFW_RELEASE) {
      mouse.m_Released[Input::MouseButton::FromGLFW(button)] = true;
      mouse.m_Held[Input::MouseButton::FromGLFW(button)] = false;
    }
  }
}
