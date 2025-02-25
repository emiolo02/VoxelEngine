#pragma once

struct BoundingBox {
    vec3 min = {}, max = {};

    BoundingBox() = default;

    BoundingBox(const vec3 &min, const vec3 &max)
        : min(min), max(max) {
    }

    // Assuming size is positive
    BoundingBox(const vec3 &center, const float size)
        : min(center - size / 2.0f), max(center + size / 2.0f) {
    }

    void SetSize(const vec3 &newSize) {
        const vec3 c = GetCenter();
        min = c - newSize;
        max = c + newSize;
    }

    vec3 GetCenter() const {
        return (min + max) * 0.5f;
    }

    vec3 GetSize() const {
        return max - min;
    }

    void GrowToInclude(const vec3 &point) {
        min = {
            std::min(min.x, point.x),
            std::min(min.y, point.y),
            std::min(min.z, point.z)
        };
        max = {
            std::max(max.x, point.x),
            std::max(max.y, point.y),
            std::max(max.z, point.z)
        };
    }
};

