#pragma once
#include <functional>
#include <unordered_map>

struct InspTexture {
    uint32 id = 0;
    int32 width = 0, height = 0;
    float scale = 0.0f;
};

struct InspFloat {
    float f = 0.0f;
    float step = 0.1f;
};

class Inspector {
public:
    void Draw();

    void AddInt(const string &name, const int value = 0) {
        m_Ints[name] = value;
    }

    int GetInt(const string &name) {
        if (!m_Ints.contains(name)) {
            return 0;
        }

        return m_Ints[name];
    }

    void AddBool(const string &name, const bool value = false) {
        m_Bools[name] = value;
    }

    bool GetBool(const string &name) {
        if (m_Bools.find(name) == m_Bools.end()) {
            LOG(name << "does not exist in Inspector m_Bools!")
            return false;
        }

        return m_Bools[name];
    }

    void AddFloat(const string &name, const float value = 0.0f, const float step = 0.1f) {
        m_Floats[name] = {value, step};
    }

    void AddVec2(const string &name, const vec2 &v) {
        m_Vec2s[name] = v;
    }

    void AddVec3(const string &name, const vec3 &v) {
        m_Vec3s[name] = v;
    }

    void AddVec4(const string &name, const vec4 &v) {
        m_Vec4s[name] = v;
    }

    void AddTexture(const string &name, const InspTexture &texture) {
        m_Textures[name] = texture;
    }

    float GetFloat(const string &name) {
        if (m_Floats.find(name) == m_Floats.end()) {
            LOG(name << "does not exist in Inspector m_Floats!")
            return 0.0f;
        }

        return m_Floats[name].f;
    }

    void SetFloat(const string &name, const float value) {
        if (m_Floats.find(name) == m_Floats.end()) {
            LOG(name << "does not exist in Inspector m_Floats!")
            return;
        }
        m_Floats[name].f = value;
    }

    void AddButton(const string &name, const std::function<void()> &func) {
        m_Buttons[name] = func;
    }

private:
    std::unordered_map<string, int32> m_Ints;
    std::unordered_map<string, bool> m_Bools;
    std::unordered_map<string, InspFloat> m_Floats;
    std::unordered_map<string, vec2> m_Vec2s;
    std::unordered_map<string, vec3> m_Vec3s;
    std::unordered_map<string, vec4> m_Vec4s;
    std::unordered_map<string, InspTexture> m_Textures;
    std::unordered_map<string, std::function<void()> > m_Buttons;

    bool m_ShowViewport = true;
};
