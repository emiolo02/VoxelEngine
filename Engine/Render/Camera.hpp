#pragma once

class Camera {
public:
  Camera();

  void Perspective(float near, float far, float aspect, float fovY);

  void Orthographic(float near, float far, float l, float r, float t, float b);

  void LookAt(const vec3 &eye, const vec3 &target, const vec3 &up);

  float GetFOVdeg() const { return m_FovY; }
  float GetFOVrad() const { return m_FovY * fPI / 180.0f; }
  float GetAspect() const { return m_Aspect; }

  const vec3 &GetPosition() const { return m_Position; }

  const mat4 &GetProjection() const { return m_Projection; }
  const mat4 &GetView() const { return m_View; }
  const mat4 &GetProjView() const { return m_ProjView; }

  const mat4 &GetInvProjection() const { return m_InvProjection; }
  const mat4 &GetInvView() const { return m_InvView; }
  const mat4 &GetInvProjView() const { return m_InvProjView; }

  static Camera *GetMainCamera() { return s_MainCamera; }
  static void SetMainCamera(Camera *camera) { s_MainCamera = camera; }

private:
  vec3 m_Position;
  mat4 m_Projection;
  mat4 m_View;
  mat4 m_ProjView;

  mat4 m_InvProjection;
  mat4 m_InvView;
  mat4 m_InvProjView;

  float m_Near = 0.0f;
  float m_Far = 0.0f;
  float m_FovY = 0.0f;
  float m_Aspect = 0.0f;

  static Camera *s_MainCamera;
};
