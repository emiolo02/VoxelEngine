#pragma once

struct Transform {
    vec3 position = vec3();
    vec3 scale = vec3();
    quat orientation = identity<quat>();

    mat4 GetMatrix() const {
        return glm::translate(identity<mat4>(), position) * mat4(orientation) * glm::scale(identity<mat4>(), scale);
    }
};
