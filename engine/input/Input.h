#pragma once
#include "Keyboard.h"

namespace Input
{

    struct Keyboard
    {
        bool pressed[Input::Key::NumKeyCodes];
        bool held[Input::Key::NumKeyCodes];
        bool released[Input::Key::NumKeyCodes];
    };

    struct Mouse
    {
        //position
        float x, y;
        float dx, dy;

        // NDC position
        float nx, ny;

        float lastX, lastY;

        //buttons
        bool pressed[Input::MouseButton::numButtons];
        bool held[Input::MouseButton::numButtons];
        bool released[Input::MouseButton::numButtons];
    };

    class InputManager
    {
    public:
        static InputManager* Instance()
        {
            static InputManager instance;
            return &instance;
        }

        Keyboard keyboard;
        Mouse mouse;

        void BeginFrame()
        {
            for (int i = 0; i < Input::Key::NumKeyCodes; i++)
            {
                keyboard.pressed[i] = false;
                keyboard.released[i] = false;
            }
            for (int i = 0; i < Input::MouseButton::numButtons; i++)
            {
                mouse.pressed[i] = false;
                mouse.released[i] = false;
            }
        }

        void HandleKeyEvent(int32 key, int32 action)
        {
            if (action == GLFW_PRESS)
            {
                keyboard.pressed[Input::Key::FromGLFW(key)] = true;
                keyboard.held[Input::Key::FromGLFW(key)] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                keyboard.released[Input::Key::FromGLFW(key)] = true;
                keyboard.held[Input::Key::FromGLFW(key)] = false;
            }
        }

        void HandleMousePosition(float64 x, float64 y, int32 w, int32 h)
        {
            mouse.nx = 2*x / w - 1.0f;
            mouse.ny = 2*y / h - 1.0f;

            mouse.x = x;
            mouse.y = y;

            mouse.dx = x - mouse.lastX;
            mouse.dy = y - mouse.lastY;

            mouse.lastX = x;
            mouse.lastY = y;
        }

        void HandleMouseButton(int32 button, int32 action)
        {
            if (action == GLFW_PRESS)
            {
                mouse.pressed[Input::MouseButton::FromGLFW(button)] = true;
                mouse.held[Input::MouseButton::FromGLFW(button)] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                mouse.released[Input::MouseButton::FromGLFW(button)] = true;
                mouse.held[Input::MouseButton::FromGLFW(button)] = false;
            }
        }

    private:
        static InputManager* instance;
        InputManager() {}
        ~InputManager() {}
    };
}