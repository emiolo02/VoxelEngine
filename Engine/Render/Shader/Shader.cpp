#include "Shader.hpp"
#include "GL/glew.h"
#include <fstream>
#include <sstream>

std::unordered_map<string, string> Shader::s_ShaderCache;

//------------------------------------------------------------------------------------------

Shader::Shader(Shader &&other) noexcept {
  m_Id = other.m_Id;
  other.m_Id = 0;
}

//------------------------------------------------------------------------------------------

Shader::Shader(const string &vsPath, const string &fsPath) {
  Load(vsPath, fsPath);
}

//------------------------------------------------------------------------------------------

Shader::Shader(const string &csPath) {
  Load(csPath);
}

//------------------------------------------------------------------------------------------

Shader::~Shader() {
  if (m_Id) {
    glDeleteProgram(m_Id);
  }
}

//------------------------------------------------------------------------------------------

Shader &
Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    m_Id = other.m_Id;
    other.m_Id = 0;
  }
  return *this;
}

//------------------------------------------------------------------------------------------

void
Shader::Load(const string &vsPath, const string &fsPath) {
  m_Id = glCreateProgram();
  const uint32 vs = CompileShader(vsPath, GL_VERTEX_SHADER);
  const uint32 fs = CompileShader(fsPath, GL_FRAGMENT_SHADER);

  glAttachShader(m_Id, vs);
  glAttachShader(m_Id, fs);
  glLinkProgram(m_Id);
}

//------------------------------------------------------------------------------------------

void
Shader::Load(const string &csPath) {
  m_Id = glCreateProgram();
  const uint32 cs = CompileShader(csPath, GL_COMPUTE_SHADER);

  glAttachShader(m_Id, cs);
  glLinkProgram(m_Id);
}

//------------------------------------------------------------------------------------------

string
Shader::LoadShader(const string &path) {
  std::ifstream f(path);
  if (f.fail()) {
    PANIC("Could not find file at: " + path);
  }

  std::stringstream contents;

  string line;
  while (std::getline(f, line)) {
    if (line.find("#include") != string::npos) {
      const size_t begin = line.find('\"', 6) + 1;
      const size_t end = line.find('\"', begin);
      const string includePath = line.substr(begin, end - begin);
      contents << LoadShader("shaders/" + includePath) << '\n';
    } else {
      contents << line << '\n';
    }
  }

  return contents.str();
}

//------------------------------------------------------------------------------------------

uint32
Shader::CompileShader(const string &path, const uint32 type) {
  const string source = LoadShader(path);

  const uint32 shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  // Errors
  const string &error = ErrorLog(shader);
  if (!error.empty())
    LOG(error + " - in \"" + path + "\"");

  return shader;
}

//------------------------------------------------------------------------------------------

string
Shader::ErrorLog(const uint32 shader) {
  string error;
  int32 shaderLogSize = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shaderLogSize);

  if (shaderLogSize > 0) {
    char *log = new char[shaderLogSize];
    glGetShaderInfoLog(shader, shaderLogSize, nullptr, log);
    error = log;
    delete[] log;
  }

  return error;
}

//------------------------------------------------------------------------------------------

void
Shader::Bind() const {
  glUseProgram(m_Id);
}

//------------------------------------------------------------------------------------------

uint32
Shader::GetProgram() const {
  return m_Id;
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const bool value) {
  glUniform1i(GetUniform(m_Id, name), value);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const int32 value) {
  glUniform1i(GetUniform(m_Id, name), value);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const uint32 value) {
  glUniform1ui(GetUniform(m_Id, name), value);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const float value) {
  glUniform1f(GetUniform(m_Id, name), value);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const vec2 &value) {
  glUniform2fv(GetUniform(m_Id, name), 1, &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const vec3 &value) {
  glUniform3fv(GetUniform(m_Id, name), 1, &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const vec4 &value) {
  glUniform4fv(GetUniform(m_Id, name), 1, &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const mat3 &value) {
  glUniformMatrix3fv(GetUniform(m_Id, name), 1, false, &value[0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const mat4 &value) {
  glUniformMatrix4fv(GetUniform(m_Id, name), 1, false, &value[0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<int32> &value) {
  glUniform1iv(GetUniform(m_Id, name), value.size(), &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<uint32> &value) {
  glUniform1uiv(GetUniform(m_Id, name), value.size(), &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<float> &value) {
  glUniform1fv(GetUniform(m_Id, name), value.size(), &value[0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<vec2> &value) {
  glUniform2fv(GetUniform(m_Id, name), value.size(), &value[0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<vec3> &value) {
  glUniform3fv(GetUniform(m_Id, name), value.size(), &value[0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<vec4> &value) {
  glUniform4fv(GetUniform(m_Id, name), value.size(), &value[0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<mat3> &value) {
  glUniformMatrix3fv(GetUniform(m_Id, name), value.size(), false, &value[0][0][0]);
}

//------------------------------------------------------------------------------------------

void
Shader::SetValue(const string &name, const std::vector<mat4> &value) {
  glUniformMatrix4fv(GetUniform(m_Id, name), value.size(), false, &value[0][0][0]);
}

//------------------------------------------------------------------------------------------

int32
Shader::GetUniform(const uint32 program, const string &name) {
  if (m_UniformCache.contains(name))
    return m_UniformCache[name];

  const int32 location = glGetUniformLocation(program, name.c_str());
  if (location == -1)
    std::cout << "Uniform \"" << name << "\" was not found!\n";

  m_UniformCache[name] = location;
  return location;
}
