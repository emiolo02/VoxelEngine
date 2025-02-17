#pragma once
#include "Keyboard.hpp"

namespace Input {
  class Keyboard {
  public:
    NON_COPYABLE(Keyboard)

    Keyboard() {
    }

    inline const bool GetPressed(Key::Code key) const { return m_Pressed[key]; }
    inline const bool GetHeld(Key::Code key) const { return m_Held[key]; }
    inline const bool GetReleased(Key::Code key) const { return m_Released[key]; }

  private:
    bool m_Pressed[Key::NumKeyCodes] = {0};
    bool m_Held[Key::NumKeyCodes] = {0};
    bool m_Released[Key::NumKeyCodes] = {0};

    friend class InputManager;
  };

  class Mouse {
  public:
    NON_COPYABLE(Mouse)

    Mouse() {
    }

    inline const vec2 &GetPosition() const { return m_Position; }
    inline const vec2 &GetLastPosition() const { return m_LastPosition; }
    inline const vec2 &GetDeltaPosition() const { return m_DeltaPosition; }

    inline const vec2 &GetNormalizedPosition() const {
      return m_NormalizedPosition;
    }

    inline const int8 GetDeltaScroll() const { return m_DeltaScroll; }

    inline const bool GetPressed(MouseButton::Code button) const {
      return m_Pressed[button];
    }

    inline const bool GetHeld(MouseButton::Code button) const {
      return m_Held[button];
    }

    inline const bool GetReleased(MouseButton::Code button) const {
      return m_Released[button];
    }

  private:
    vec2 m_Position = vec2();
    vec2 m_LastPosition = vec2();
    vec2 m_DeltaPosition = vec2();
    vec2 m_NormalizedPosition = vec2();

    int8 m_DeltaScroll = 0;

    // buttons
    bool m_Pressed[MouseButton::numButtons] = {0};
    bool m_Held[MouseButton::numButtons] = {0};
    bool m_Released[MouseButton::numButtons] = {0};

    friend class InputManager;
  };

  class InputManager {
    SINGLETON(InputManager)

  public:
    Keyboard keyboard;
    Mouse mouse;

    void BeginFrame();

    void HandleKeyEvent(int32 key, int32 action);

    void HandleMousePosition(float64 x, float64 y, int32 w, int32 h);

    void HandleMouseScroll(float64 x, float64 y);

    void HandleMouseButton(int32 button, int32 action);
  };
}
