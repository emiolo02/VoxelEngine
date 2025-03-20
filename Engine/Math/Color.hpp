#pragma once

namespace math {
    struct Color {
        union {
            struct {
                uint8 a, b, g, r;
            };

            uint32 data = 0;
        };

        Color() = default;

        explicit Color(uint32 color);

        explicit Color(const vec3 &color);

        explicit Color(const vec4 &color);

        Color(float r, float g, float b, float a);

        Color(uint8 r, uint8 g, uint8 b, uint8 a);

        Color &operator=(const Color &other);
    };
}
