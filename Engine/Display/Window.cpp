//------------------------------------------------------------------------------
// window.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "Window.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

namespace Display {
#ifdef __WIN32__
#define APICALLTYPE __stdcall
#else
#define APICALLTYPE
#endif

    //------------------------------------------------------------------------------
    /**
     */
    static void GLAPIENTRY
    GLDebugCallback(GLenum source,
                    GLenum type,
                    GLuint id,
                    GLenum severity,
                    GLsizei length,
                    const GLchar *message,
                    const void *userParam) {
        std::string msg("[OPENGL DEBUG MESSAGE] ");

        // print error severity
        switch (severity) {
            case GL_DEBUG_SEVERITY_LOW:
                msg.append("<Low severity> ");
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                msg.append("<Medium severity> ");
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                msg.append("<High severity> ");
                break;
        }

        // append message to output
        msg.append(message);

        // print message
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                printf("Error: %s\n", msg.c_str());
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                printf("Performance issue: %s\n", msg.c_str());
                break;
            default: // Portability, Deprecated, Other
                break;
        }
        assert(/*severity != GL_DEBUG_SEVERITY_MEDIUM && */severity != GL_DEBUG_SEVERITY_HIGH);
    }

    static void GLFWErrorCallback(int error_code, const char *description) {
        std::cerr << description << '\n';
        exit(error_code);
    }

    int32 Window::s_WindowCount = 0;
    //------------------------------------------------------------------------------
    /**
     */
    Window::Window()
        : m_Window(nullptr)
          , m_Width(1024)
          , m_Height(768)
          , m_Title("gscept Lab Environment") {
        // empty
    }

    //------------------------------------------------------------------------------
    /**
     */
    Window::~Window() {
        // empty
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticKeyPressCallback(GLFWwindow *win,
                                   int32 key,
                                   int32 scancode,
                                   int32 action,
                                   int32 mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (ImGui::GetIO().WantCaptureMouse) {
            ImGui_ImplGlfw_KeyCallback(win, key, scancode, action, mods);
        } else if (nullptr != window->m_KeyPressCallback) {
            window->m_KeyPressCallback(key, scancode, action, mods);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticMousePressCallback(GLFWwindow *win,
                                     int32 button,
                                     int32 action,
                                     int32 mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (ImGui::GetIO().WantCaptureMouse) {
            ImGui_ImplGlfw_MouseButtonCallback(win, button, action, mods);
        } else if (nullptr != window->m_MousePressCallback) {
            window->m_MousePressCallback(button, action, mods);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticMouseMoveCallback(GLFWwindow *win, float64 x, float64 y) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (nullptr != window->m_MouseMoveCallback) {
            window->m_MouseMoveCallback(x, y);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticMouseEnterLeaveCallback(GLFWwindow *win, int32 mode) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (nullptr != window->m_MouseLeaveEnterCallback) {
            window->m_MouseLeaveEnterCallback(mode == 0);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticMouseScrollCallback(GLFWwindow *win, float64 x, float64 y) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (ImGui::GetIO().WantCaptureMouse) {
            ImGui_ImplGlfw_ScrollCallback(win, x, y);
        } else if (nullptr != window->m_MouseScrollCallback) {
            window->m_MouseScrollCallback(x, y);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticCloseCallback(GLFWwindow *win) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        window->Close();
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::StaticResizeCallback(GLFWwindow *win, int32 width, int32 height) {
        Window *window = (Window *) glfwGetWindowUserPointer(win);
        if (window->m_ResizeCallback != nullptr)
            window->m_ResizeCallback(width, height);
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::Resize() {
        if (nullptr != m_Window) {
            glfwSetWindowSize(m_Window, m_Width, m_Height);

            // setup viewport
            glViewport(0, 0, m_Width, m_Height);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::Retitle() {
        if (nullptr != m_Window) {
            glfwSetWindowTitle(m_Window, m_Title.c_str());
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    bool
    Window::Open() {
        glfwSetErrorCallback(GLFWErrorCallback);
        if (Window::s_WindowCount == 0) {
            if (!glfwInit()) {
                return false;
            }
        }

        // setup window
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        //glEnable(GL_DEBUG_OUTPUT);
        //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef CI_TEST
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif

        // open window
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

        if (m_Window == nullptr) {
            printf("[ERROR]: Could not create GLFW window!\n");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        if (nullptr != m_Window && s_WindowCount == 0) {
            GLenum res = glewInit();

            assert(res == GLEW_OK);
            if (!GLEW_VERSION_4_0) {
                printf("[ERROR]: OpenGL 4.0+ is not supported on this hardware!\n");
                glfwDestroyWindow(m_Window);
                m_Window = nullptr;
                glfwTerminate();
                return false;
            }

            std::cout << "Detected hardware:\n" <<
                    '\t' << glGetString(GL_VENDOR) << '\n' <<
                    '\t' << glGetString(GL_RENDERER) << '\n';
            std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

            // setup debug callback
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugCallback, NULL);
            GLuint unusedIds = 0;
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);

            // setup stuff
            glEnable(GL_FRAMEBUFFER_SRGB);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_POLYGON_SMOOTH);
            glEnable(GL_MULTISAMPLE);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

            // setup viewport
            glViewport(0, 0, m_Width, m_Height);
        }

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetKeyCallback(m_Window, Window::StaticKeyPressCallback);
        glfwSetMouseButtonCallback(m_Window, Window::StaticMousePressCallback);
        glfwSetCursorPosCallback(m_Window, Window::StaticMouseMoveCallback);
        glfwSetCursorEnterCallback(m_Window, Window::StaticMouseEnterLeaveCallback);
        glfwSetScrollCallback(m_Window, Window::StaticMouseScrollCallback);
        glfwSetWindowCloseCallback(m_Window, Window::StaticCloseCallback);
        glfwSetWindowSizeCallback(m_Window, Window::StaticResizeCallback);
        // setup imgui implementation
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_Window, false);
        glfwSetCharCallback(m_Window, ImGui_ImplGlfw_CharCallback);
        // increase window count and return result
        Window::s_WindowCount++;
        return m_Window != nullptr;
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::Close() {
        if (nullptr != m_Window) {
            glfwDestroyWindow(m_Window);
        }
        m_Window = nullptr;
        Window::s_WindowCount--;
        if (Window::s_WindowCount == 0) {
            ImGui_ImplGlfw_Shutdown();
            glfwTerminate();
        }
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::MakeCurrent() {
        glfwMakeContextCurrent(m_Window);
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::Update() {
        glfwPollEvents();
    }

    //------------------------------------------------------------------------------
    /**
     */
    void
    Window::SwapBuffers() {
        if (m_Window) {
            if (nullptr != m_UiFunc) {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                m_UiFunc();
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }
            glfwSwapBuffers(m_Window);
        }
    }

    //------------------------------------------------------------------------------
    /**
     */

    void
    Window::HideCursor() {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void
    Window::LockCursor() {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
} // namespace Display
