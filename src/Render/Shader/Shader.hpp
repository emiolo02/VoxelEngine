#pragma once

#include <unordered_map>

class Shader {
public:
  Shader() = default;

  Shader(const string &vsPath, const string &fsPath);

  explicit Shader(const string &csPath);

  ~Shader();

  void Load(const string &vsPath, const string &fsPath);

  void Load(const string &csPath);

  void Bind() const;

  void SetValue(const string &name, bool value);

  void SetValue(const string &name, int32 value);

  void SetValue(const string &name, uint32 value);

  void SetValue(const string &name, float value);

  void SetValue(const string &name, const vec2 &value);

  void SetValue(const string &name, const vec3 &value);

  void SetValue(const string &name, const vec4 &value);

  void SetValue(const string &name, const mat3 &value);

  void SetValue(const string &name, const mat4 &value);

  void SetValue(const string &name, const std::vector<int32> &value);

  void SetValue(const string &name, const std::vector<uint32> &value);

  void SetValue(const string &name, const std::vector<float> &value);

  void SetValue(const string &name, const std::vector<vec2> &value);

  void SetValue(const string &name, const std::vector<vec3> &value);

  void SetValue(const string &name, const std::vector<vec4> &value);

  void SetValue(const string &name, const std::vector<mat3> &value);

  void SetValue(const string &name, const std::vector<mat4> &value);

private:
  static string LoadShader(const string &path);

  static uint32 CompileShader(const string &path, uint32 type);

  static string ErrorLog(uint32 shader);

  int32 GetUniform(const string &name);

  std::unordered_map<string, string> m_ShaderCache;
  std::unordered_map<string, int32> m_UniformCache;
  uint32 m_Id = 0;
};
