#include "Color.hpp"

namespace math {
    //------------------------------------------------------------------------------------------

    Color::Color(const uint32 color)
        : data(color) {
    }

    //------------------------------------------------------------------------------------------

    Color::Color(const vec4 &color)
        : a(static_cast<uint8>(clamp(color.a, 0.0f, 1.0f) * 255.0f)),
          b(static_cast<uint8>(clamp(color.b, 0.0f, 1.0f) * 255.0f)),
          g(static_cast<uint8>(clamp(color.g, 0.0f, 1.0f) * 255.0f)),
          r(static_cast<uint8>(clamp(color.r, 0.0f, 1.0f) * 255.0f)) {
    }

    //------------------------------------------------------------------------------------------

    Color::Color(const float r, const float g, const float b, const float a)
        : a(static_cast<uint8>(clamp(a, 0.0f, 1.0f) * 255.0f)),
          b(static_cast<uint8>(clamp(b, 0.0f, 1.0f) * 255.0f)),
          g(static_cast<uint8>(clamp(g, 0.0f, 1.0f) * 255.0f)),
          r(static_cast<uint8>(clamp(r, 0.0f, 1.0f) * 255.0f)) {
    }

    //------------------------------------------------------------------------------------------

    Color::Color(const uint8 r, const uint8 g, const uint8 b, const uint8 a)
        : a(a),
          b(b),
          g(g),
          r(r) {
    }

    Color &
    Color::operator=(const Color &other) {
        data = other.data;
        return *this;
    }

    //------------------------------------------------------------------------------------------
}
