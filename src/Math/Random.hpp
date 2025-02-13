#pragma once
namespace math::random {
    void SetSeed(uint32 seed);

    uint32 FastRandom();

    uint32 FastRandom(uint32 seed);

    float RandomFloat();

    float RandomFloat(uint32 seed);
}
