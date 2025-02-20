#include "Camera.hpp"

Camera *Camera::s_MainCamera = nullptr;

//------------------------------------------------------------------------------------------

Camera::Camera()
  : m_Position(0.0f),
    m_Projection(identity<mat4>()),
    m_View(identity<mat4>()),
    m_ProjView(identity<mat4>()),
    m_InvProjection(identity<mat4>()),
    m_InvView(identity<mat4>()),
    m_InvProjView(identity<mat4>()) {
}

//------------------------------------------------------------------------------------------

void
Camera::Perspective(const float near, const float far, const float aspect, const float fovY) {
  m_Near = near, m_Far = far;
  m_FovY = fovY;
  m_Aspect = aspect;

  m_Projection = perspective(fovY, aspect, near, far);
  m_InvProjection = inverse(m_Projection);

  m_ProjView = m_Projection * m_View;
  m_InvProjView = m_InvProjection * m_InvView;
}

//------------------------------------------------------------------------------------------

void
Camera::Orthographic(const float near, const float far, const float l, const float r, const float t, const float b) {
  m_Near = near, m_Far = far;
  m_FovY = 0.0f;
  m_Aspect = t / r;

  m_Projection = ortho(l, r, b, t, near, far);
  m_ProjView = m_Projection * m_View;

  m_InvProjection = inverse(m_Projection);
  m_InvProjView = m_InvProjection * m_InvView;
}

//------------------------------------------------------------------------------------------

void
Camera::LookAt(const vec3 &eye, const vec3 &target, const vec3 &up) {
  m_Position = eye;

  m_View = lookAt(eye, target, up);
  m_ProjView = m_Projection * m_View;
  m_InvView = inverse(m_View);
  m_InvProjView = m_InvProjection * m_InvView;
}
