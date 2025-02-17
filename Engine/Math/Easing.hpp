#pragma once

#include <cmath>

namespace math {
    inline float
    EaseInSine(const float x) {
        return 1.0f - std::cos(x * fPI / 2.0f);
    }

    inline float
    EaseOutSine(const float x) {
        return std::sin(x * fPI / 2.0f);
    }

    inline float
    EaseInOutSine(const float x) {
        return -(std::cos(fPI * x) - 1.0f) / 2.0f;
    }

    inline float
    Lerp(const float x, const float y, const float t) {
        return x + t * (y - x);
    }

    inline float
    Clamp(const float x, const float lowerlimit = 0.0f, const float upperlimit = 1.0f) {
        if (x < lowerlimit) return lowerlimit;
        if (x > upperlimit) return upperlimit;
        return x;
    }

    inline float
    Smoothstep(const float edge0, const float edge1, float x) {
        // Scale, and clamp x to 0..1 range
        x = Clamp((x - edge0) / (edge1 - edge0));

        return x * x * (3.0f - 2.0f * x);
    }
}
