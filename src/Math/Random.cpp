#include "Random.hpp"
#include "Easing.hpp"

namespace math::random {
    thread_local uint32 rseed = 0;

    void
    SetSeed(const uint32 seed) {
        rseed = seed;
    }

    uint32
    FastRandom() {
        return rseed = rseed * 1103515245u + 12345u & INT_MAX;
    }

    uint32
    FastRandom(const uint32 seed) {
        return seed * 1103515245u + 12345u & INT_MAX;
    }

    float
    RandomFloat() {
        thread_local union {
            unsigned int i;
            float f;
        } r;
        r.i = FastRandom() & 0x007fffff | 0x3f800000;
        return r.f - 1.0f;
    }

    float
    RandomFloat(const uint32 seed) {
        thread_local union {
            unsigned int i;
            float f;
        } r;
        r.i = FastRandom(seed) & 0x007fffff | 0x3f800000;
        return r.f - 1.0f;
    }
}
