#include "Shader.hpp"
#include "GL/glew.h"
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------------------

Shader &
Shader::operator=(const Shader &other) {
    glDeleteProgram(m_Id);
    m_Id = other.m_Id;
    m_UniformCache = other.m_UniformCache;

    return *this;
}

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

//------------------------------------------------------------------------------------------

Shader &
ShaderManager::Load(const string &vsPath, const string &fsPath) {
    Shader shader;
    shader.m_Id = glCreateProgram();
    const uint32 vs = CompileShader(vsPath, GL_VERTEX_SHADER);
    const uint32 fs = CompileShader(fsPath, GL_FRAGMENT_SHADER);

    glAttachShader(shader.m_Id, vs);
    glAttachShader(shader.m_Id, fs);
    glLinkProgram(shader.m_Id);

    m_Shaders.push_back(shader);

    return m_Shaders.back();
}

//------------------------------------------------------------------------------------------

Shader &
ShaderManager::Load(const string &csPath) {
    Shader shader;
    shader.m_Id = glCreateProgram();
    const uint32 cs = CompileShader(csPath, GL_COMPUTE_SHADER);

    glAttachShader(shader.m_Id, cs);
    glLinkProgram(shader.m_Id);

    m_Shaders.push_back(shader);

    return m_Shaders.back();
}

void
ShaderManager::Clear() {
    for (const auto &shader: m_Shaders) {
        glDeleteProgram(shader.m_Id);
    }

    m_Shaders.clear();
}

//------------------------------------------------------------------------------------------

string
ShaderManager::LoadShader(const string &path) {
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
ShaderManager::CompileShader(const string &path, const uint32 type) {
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
ShaderManager::ErrorLog(const uint32 shader) {
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
