#pragma once

namespace math {
    struct Ray;

    struct BoundingBox {
        vec3 min = {}, max = {};

        BoundingBox() = default;

        BoundingBox(const vec3 &min, const vec3 &max);

        BoundingBox(const vec3 &center, float size);

        void SetSize(const vec3 &newSize);

        vec3 GetCenter() const;

        vec3 GetSize() const;

        void GrowToInclude(const vec3 &point);

        bool Intersect(const Ray &ray) const;

        bool Intersect(const Ray &ray, float &tNear, float &tFar) const;
    };
}
