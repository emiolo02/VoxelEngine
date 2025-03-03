#include "Ray.hpp"
#include "BoundingBox.hpp"

namespace math {
    //------------------------------------------------------------------------------------------

    bool
    Ray::Intersect(const BoundingBox &boundingBox) const {
        const vec3 tMin = (boundingBox.min - this->origin) / this->direction;
        const vec3 tMax = (boundingBox.max - this->origin) / this->direction;
        const vec3 t1 = min(tMin, tMax);
        const vec3 t2 = max(tMin, tMax);
        const float tNear = std::max(std::max(t1.x, t1.y), t1.z);
        const float tFar = std::max(std::max(t2.x, t2.y), t2.z);
        return tNear < tFar && tFar > 0.0f;
    }

    //------------------------------------------------------------------------------------------

    bool
    Ray::Intersect(const BoundingBox &boundingBox, float &tNear, float &tFar) const {
        const vec3 tMin = (boundingBox.min - this->origin) / this->direction;
        const vec3 tMax = (boundingBox.max - this->origin) / this->direction;
        const vec3 t1 = min(tMin, tMax);
        const vec3 t2 = max(tMin, tMax);
        tNear = std::max(std::max(t1.x, t1.y), t1.z);
        tFar = std::max(std::max(t2.x, t2.y), t2.z);
        return tNear < tFar && tFar > 0.0f;
    }

    //------------------------------------------------------------------------------------------
}
