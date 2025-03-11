#include "BoundingBox.hpp"
#include "Ray.hpp"

namespace math {
    //------------------------------------------------------------------------------------------

    BoundingBox::BoundingBox(const vec3 &min, const vec3 &max)
        : min(min), max(max) {
    }

    //------------------------------------------------------------------------------------------

    BoundingBox::BoundingBox(const vec3 &center, const float size)
        : min(center - size * 0.5f), max(center + size * 0.5f) {
        assert(size >= 0.0f);
    }

    //------------------------------------------------------------------------------------------

    void
    BoundingBox::SetSize(const vec3 &newSize) {
        const vec3 c = GetCenter();
        min = c - newSize * 0.5f;
        max = c + newSize * 0.5f;
    }

    //------------------------------------------------------------------------------------------

    vec3
    BoundingBox::GetCenter() const {
        return (min + max) * 0.5f;
    }

    //------------------------------------------------------------------------------------------

    vec3
    BoundingBox::GetSize() const {
        return max - min;
    }

    //------------------------------------------------------------------------------------------

    vec3
    BoundingBox::GetNormal(const vec3 &point) const {
        const bvec3 hitMin = lessThan(abs(point - min), vec3(0));
        const bvec3 hitMax = lessThan(abs(point - max), vec3(0));
        return vec3(hitMax) - vec3(hitMin);
    }

    //------------------------------------------------------------------------------------------

    void
    BoundingBox::GrowToInclude(const vec3 &point) {
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

    //------------------------------------------------------------------------------------------

    bool
    BoundingBox::Intersect(const Ray &ray) const {
        const vec3 tMin = (this->min - ray.origin) / ray.direction;
        const vec3 tMax = (this->max - ray.origin) / ray.direction;
        const vec3 t1 = glm::min(tMin, tMax);
        const vec3 t2 = glm::max(tMin, tMax);
        const float tNear = std::max(std::max(t1.x, t1.y), t1.z);
        const float tFar = std::max(std::max(t2.x, t2.y), t2.z);
        return tNear < tFar && tFar > 0.0f;
    }

    //------------------------------------------------------------------------------------------

    bool
    BoundingBox::Intersect(const Ray &ray, float &tNear, float &tFar) const {
        const vec3 tMin = (this->min - ray.origin) / ray.direction;
        const vec3 tMax = (this->max - ray.origin) / ray.direction;
        const vec3 t1 = glm::min(tMin, tMax);
        const vec3 t2 = glm::max(tMin, tMax);
        tNear = std::max(std::max(t1.x, t1.y), t1.z);
        tFar = std::max(std::max(t2.x, t2.y), t2.z);
        return tNear < tFar && tFar > 0.0f;
    }

    //------------------------------------------------------------------------------------------
}
