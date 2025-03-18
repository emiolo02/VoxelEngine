#pragma once

#include <unordered_map>

class Shader {
public:
    Shader &operator=(const Shader &other);

    void Bind() const;

    uint32 GetProgram() const;

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
    int32 GetUniform(uint32 program, const string &name);

    uint32 m_Id = 0;

    std::unordered_map<string, int32> m_UniformCache;

    friend class ShaderManager;
};

//------------------------------------------------------------------------------------------

class ShaderManager {
    SINGLETON(ShaderManager)

public:
    Shader &Load(const std::string &vsPath, const std::string &fsPath);

    Shader &Load(const std::string &csPath);

    void Clear();

private:
    static string LoadShader(const string &path);

    static uint32 CompileShader(const string &path, uint32 type);

    static string ErrorLog(uint32 shader);

    std::vector<Shader> m_Shaders;
};
