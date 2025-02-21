#pragma once

#include <unordered_map>

class Shader {
public:
  Shader() = default;

  Shader(Shader &&other) noexcept;

  Shader(const string &vsPath, const string &fsPath);

  explicit Shader(const string &csPath);

  ~Shader();

  Shader &operator=(Shader &&other) noexcept;

  void Load(const string &vsPath, const string &fsPath);

  void Load(const string &csPath);

  void Bind() const;

  uint32 GetProgram() const;

  void SetValue(const string &name, bool value) const;

  void SetValue(const string &name, int32 value) const;

  void SetValue(const string &name, uint32 value) const;

  void SetValue(const string &name, float value) const;

  void SetValue(const string &name, const vec2 &value) const;

  void SetValue(const string &name, const vec3 &value) const;

  void SetValue(const string &name, const vec4 &value) const;

  void SetValue(const string &name, const mat3 &value) const;

  void SetValue(const string &name, const mat4 &value) const;

  void SetValue(const string &name, const std::vector<int32> &value) const;

  void SetValue(const string &name, const std::vector<uint32> &value) const;

  void SetValue(const string &name, const std::vector<float> &value) const;

  void SetValue(const string &name, const std::vector<vec2> &value) const;

  void SetValue(const string &name, const std::vector<vec3> &value) const;

  void SetValue(const string &name, const std::vector<vec4> &value) const;

  void SetValue(const string &name, const std::vector<mat3> &value) const;

  void SetValue(const string &name, const std::vector<mat4> &value) const;

private:
  static string LoadShader(const string &path);

  static uint32 CompileShader(const string &path, uint32 type);

  static string ErrorLog(uint32 shader);

  static int32 GetUniform(uint32 program, const string &name);

  static std::unordered_map<string, string> s_ShaderCache;
  static std::unordered_map<string, int32> s_UniformCache;
  uint32 m_Id = 0;
};
