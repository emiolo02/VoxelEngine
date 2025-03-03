#pragma once

namespace math {
    struct BoundingBox;

    struct Ray {
        vec3 origin{};
        vec3 direction{};

        bool Intersect(const BoundingBox &boundingBox) const;

        bool Intersect(const BoundingBox &boundingBox, float &tNear, float &tFar) const;
    };
}
