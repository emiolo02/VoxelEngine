#include "Terrain.hpp"
#include "Math/PerlinNoise.hpp"
#include "Math/Random.hpp"

void
GenerateTerrain(BrickMap &bm, const uint32 dims, const float invScale) {
    constexpr float heightScale = 96.0f;
    bm = BrickMap(vec3(), ivec3(dims, dims / 2, dims), 0.1f);
    siv::PerlinNoise heightMap;


    for (uint32 z = 0; z < dims; ++z) {
        for (uint32 x = 0; x < dims; ++x) {
            const float fx = static_cast<float>(x) / invScale;
            const float fz = static_cast<float>(z) / invScale;
            const float height = heightMap.octave2D_01(fx, fz, 8);
            int y = height * (height * 1.2) * heightScale;

            const uint8 gr = 24 + static_cast<uint8>(math::random::RandomFloat() * 24);
            const uint8 gg = 124 + static_cast<uint8>(math::random::RandomFloat() * 44);
            const uint8 gb = 24 + static_cast<uint8>(math::random::RandomFloat() * 24);
            const Color grass = {gr, gg, gb, 255};
            bm.Insert(x, y, z, grass);
            while (y-- > 0) {
                const uint8 dr = 80 + static_cast<uint8>(math::random::RandomFloat() * 24);
                const uint8 dg = 30 + static_cast<uint8>(math::random::RandomFloat() * 24);
                const uint8 db = 10 + static_cast<uint8>(math::random::RandomFloat() * 12);
                const Color dirt = {dr, dg, db, 255};
                bm.Insert(x, y, z, dirt);
            }
        }
    }
}
