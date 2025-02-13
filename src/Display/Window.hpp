#pragma once

#include <functional>
#include <string>

struct GLFWwindow;

namespace Display {

class Window
{
public:
  Window();
  ~Window();

  void SetSize(int32 width, int32 height, bool resize = true);
  void GetSize(int32& width, int32& height);
  void SetTitle(const std::string& title);

  bool Open();
  void Close();
  const bool IsOpen() const;

  /// bit block transfer from buffer to screen. data buffer must be exactly w *
  /// h * 3 large!
  void Blit(uint8* data, int w, int h);

  /// make this window current, meaning all draws will direct to this window
  /// context
  void MakeCurrent();

  void Update();
  void SwapBuffers();

  void SetKeyPressFunction(
    const std::function<void(int32, int32, int32, int32)>& func);
  void SetMousePressFunction(
    const std::function<void(int32, int32, int32)>& func);
  void SetMouseMoveFunction(const std::function<void(float64, float64)>& func);
  void SetMouseEnterLeaveFunction(const std::function<void(bool)>& func);
  void SetMouseScrollFunction(
    const std::function<void(float64, float64)>& func);
  void SetWindowResizeFuncion(const std::function<void(int32, int32)>& func);
  void SetUiRender(const std::function<void()>& func);

  void HideCursor();
  void LockCursor();

private:
  static void StaticKeyPressCallback(GLFWwindow* win,
                                     int32 key,
                                     int32 scancode,
                                     int32 action,
                                     int32 mods);
  static void StaticMousePressCallback(GLFWwindow* win,
                                       int32 button,
                                       int32 action,
                                       int32 mods);
  static void StaticMouseMoveCallback(GLFWwindow* win, float64 x, float64 y);
  static void StaticMouseEnterLeaveCallback(GLFWwindow* win, int32 mode);
  static void StaticMouseScrollCallback(GLFWwindow* win, float64 x, float64 y);
  static void StaticCloseCallback(GLFWwindow* win);
  static void StaticResizeCallback(GLFWwindow* win, int32 width, int32 height);

  /// resize update
  void Resize();
  /// title rename update
  void Retitle();

  static int32 s_WindowCount;

  std::function<void(int32, int32, int32, int32)> m_KeyPressCallback;
  std::function<void(int32, int32, int32)> m_MousePressCallback;
  std::function<void(float64, float64)> m_MouseMoveCallback;
  std::function<void(bool)> m_MouseLeaveEnterCallback;
  std::function<void(float64, float64)> m_MouseScrollCallback;
  std::function<void()> m_UiFunc;
  std::function<void(int32, int32)> m_ResizeCallback;

  int32 m_Width;
  int32 m_Height;
  string m_Title;
  GLFWwindow* m_Window;
};

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetSize(int32 width, int32 height, bool resize)
{
  m_Width = width;
  m_Height = height;
  if (m_Window != nullptr && resize) {
    Resize();
  }
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::GetSize(int32& width, int32& height)
{
  width = m_Width;
  height = m_Height;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetTitle(const std::string& title)
{
  m_Title = title;
  if (m_Window != nullptr) {
    this->Retitle();
  }
}

//------------------------------------------------------------------------------
/**
 */
inline const bool
Window::IsOpen() const
{
  return m_Window != nullptr;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetKeyPressFunction(
  const std::function<void(int32, int32, int32, int32)>& func)
{
  m_KeyPressCallback = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetMousePressFunction(
  const std::function<void(int32, int32, int32)>& func)
{
  m_MousePressCallback = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetMouseMoveFunction(const std::function<void(float64, float64)>& func)
{
  m_MouseMoveCallback = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetMouseEnterLeaveFunction(const std::function<void(bool)>& func)
{
  m_MouseLeaveEnterCallback = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetMouseScrollFunction(
  const std::function<void(float64, float64)>& func)
{
  m_MouseScrollCallback = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetUiRender(const std::function<void()>& func)
{
  m_UiFunc = func;
}

//------------------------------------------------------------------------------
/**
 */
inline void
Window::SetWindowResizeFuncion(const std::function<void(int32, int32)>& func)
{
  m_ResizeCallback = func;
}

} // namespace Display
