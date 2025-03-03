#include "BrickMap.hpp"
#include <cmath>

#include "Math/PerlinNoise.hpp"

BrickMap::BrickMap(vec3 position, ivec3 dimensions, float voxelSize)
    : m_VoxelSize(voxelSize) {
    vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
    m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                         std::ceil(fCoarseDimensions.y),
                         std::ceil(fCoarseDimensions.z));

    m_BoundingBox = math::BoundingBox(position, vec3(m_Dimensions) * 8.0f * voxelSize + position);

    m_Grid.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z, EMPTY_BRICK);
}

BrickMap::BrickMap(const vec3 &position, const std::vector<math::Color> &voxels, const ivec3 &dimensions,
                   const float voxelSize)
    : m_VoxelSize(voxelSize) {
    const vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
    m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                         std::ceil(fCoarseDimensions.y),
                         std::ceil(fCoarseDimensions.z));

    m_BoundingBox = math::BoundingBox(position, vec3(m_Dimensions) * 8.0f * voxelSize + position);

    m_Grid.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z, EMPTY_BRICK);

    for (uint32 z = 0; z < dimensions.z; ++z) {
        for (uint32 y = 0; y < dimensions.y; ++y) {
            for (uint32 x = 0; x < dimensions.x; ++x) {
                const uint32 index = Flatten({x, y, z}, dimensions);
                if (voxels[index].data != 0) {
                    Insert(x, y, z, voxels[index]);
                }
            }
        }
    }
}

BrickMap::BrickMap(const ivec3 &dimensions, const float voxelSize)
    : m_VoxelSize(voxelSize) {
    vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
    m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                         std::ceil(fCoarseDimensions.y),
                         std::ceil(fCoarseDimensions.z));

    m_BoundingBox = math::BoundingBox(vec3(0.0f), vec3(m_Dimensions) * 8.0f * voxelSize);

    m_Grid.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z, EMPTY_BRICK);
}

std::string
PrefixedSize(const float size) {
    if (size > 1e9f)
        return std::to_string(size / 1e9f) + " gb";
    if (size > 1e6f)
        return std::to_string(size / 1e6f) + " mb";
    if (size > 1e3f)
        return std::to_string(size / 1e3f) + " kb";

    return std::to_string(size) + " b";
}

void
BrickMap::PrintByteSize() const {
    const size_t sizeGrid = m_Grid.size() * sizeof(uint32);
    const size_t sizeBricks = m_Bricks.size() * sizeof(Brick);
    const size_t sizeTextures = m_Textures.size() * sizeof(BrickTexture);
    std::cout << "Size of brick map: " << PrefixedSize(sizeGrid + sizeBricks + sizeTextures) << '\n';
    std::cout << "\tGrid:\t\t" << PrefixedSize(sizeGrid) << '\n';
    std::cout << "\tBricks:\t\t" << PrefixedSize(sizeBricks) << '\n';
    std::cout << "\tTextures:\t" << PrefixedSize(sizeTextures) << '\n';
}

struct DataDDA {
    DataDDA(const float voxelSize, const math::Ray &ray, const ivec3 &gridSize) {
        rayStart = ray.origin / voxelSize;
        position = floor(rayStart);
        const vec3 signDir = sign(ray.direction);
        gridStep = signDir;
        tDelta = signDir / ray.direction;
        tMax = (signDir * (vec3(position) - rayStart) + signDir * 0.5f + 0.5f) * tDelta;

        stepAxis = tMax.x < tMax.y ? (tMax.x < tMax.z ? 0 : 2) : tMax.y < tMax.z ? 1 : 2;
        outOfBounds = ivec3(greaterThan(ray.direction, vec3(0))) * (gridSize + 1) - 1;
    }

    void Step() {
        stepAxis = tMax.x < tMax.y ? (tMax.x < tMax.z ? 0 : 2) : tMax.y < tMax.z ? 1 : 2;
        tMax[stepAxis] += tDelta[stepAxis];
        position[stepAxis] += gridStep[stepAxis];
    }

    bool InBounds() const {
        return position.x != outOfBounds.x && position.y != outOfBounds.y && position.z != outOfBounds.z;
    }

    vec3 rayStart{};
    ivec3 outOfBounds{};
    ivec3 position{};
    ivec3 gridStep{};
    vec3 tDelta{};
    vec3 tMax{};
    int stepAxis = -1;
};

std::optional<ivec3>
BrickMap::RayCast(const math::Ray &ray) {
    hitGridCells.clear();

    float tNear, tFar;
    if (!ray.Intersect(m_BoundingBox, tNear, tFar)) {
        std::cout << "Miss box\n";
        return {};
    }
    tNear = std::max(tNear + 1e-3f, 0.0f);
    const ivec3 gridSize = m_Dimensions;
    const float brickSize = m_VoxelSize * BRICK_DIMENSIONS;
    DataDDA data(brickSize, {ray.origin + ray.direction * tNear - m_BoundingBox.min, ray.direction}, gridSize);

    while (data.InBounds()) {
        //hitGridCells.emplace_back((vec3(data.position) + vec3(0.5f)) * brickSize + m_BoundingBox.min, brickSize);
        if (m_Grid[Flatten(data.position, gridSize)] != EMPTY_BRICK) {
            //std::cout << "Found brick\n";
            vec3 lastTMax = data.tMax - data.tDelta[data.stepAxis];
            lastTMax[data.stepAxis] -= data.tDelta[data.stepAxis];

            const float hitT = std::min(std::min(lastTMax.x, lastTMax.y), lastTMax.z) + 1e-3f;
            const vec3 hitPosition = (data.rayStart + ray.direction * hitT - vec3(data.position)) * brickSize;

            const math::BoundingBox brickBounds((vec3(data.position) + vec3(0.5f)) * brickSize + m_BoundingBox.min,
                                                brickSize);
            hitGridCells.emplace_back(brickBounds);
            //return {};
            const auto hit = TraverseFine(data.position, ray, brickBounds);
            if (hit) return hit;
        }
        data.Step();
    }

    return {};
}

std::tuple<uint32 &, BrickMap::Brick &, BrickMap::BrickTexture &>
BrickMap::GetHierarchy(const ivec3 &position) {
    uint32 &cell = m_Grid[Flatten(position / 8, m_Dimensions)];
    Brick &brick = m_Bricks[cell];
    BrickTexture &texture = m_Textures[brick.colorPointer];
    return {cell, brick, texture};
}

std::optional<ivec3>
BrickMap::TraverseCoarse(const math::Ray &ray) {
    return {};
}

std::optional<ivec3>
BrickMap::TraverseFine(const ivec3 &brickPosition, const math::Ray &ray, const math::BoundingBox &brickBounds) {
    //DataDDA data(m_VoxelSize, ray, ivec3(8));
    float tNear, tFar;
    if (!ray.Intersect(brickBounds, tNear, tFar)) {
        //std::cout << "Miss box\n";
        return {};
    }
    tNear = std::max(tNear + 1e-3f, 0.0f);
    DataDDA data(m_VoxelSize, {ray.origin + ray.direction * tNear - brickBounds.min, ray.direction}, ivec3(8));

    const Brick &brick = m_Bricks[m_Grid[Flatten(brickPosition, m_Dimensions)]];

    while (data.InBounds()) {
        if (brick.VoxelAt(Flatten(data.position, ivec3(8)))) {
            hitGridCells.emplace_back((vec3(data.position) + vec3(0.5f)) * m_VoxelSize + brickBounds.min, m_VoxelSize);
            return brickPosition * 8 + data.position;
        }
        data.Step();
    }
    return {};
}

bool
BrickMap::Insert(uint32 x, uint32 y, uint32 z, math::Color color) {
    const uint32 coarseX = x / 8;
    const uint32 coarseY = y / 8;
    const uint32 coarseZ = z / 8;
    if (coarseX > m_Dimensions.x || coarseY > m_Dimensions.y || coarseZ > m_Dimensions.z) {
        return false;
    }

    const size_t coarseIndex = coarseX + m_Dimensions.x * (coarseY + m_Dimensions.y * coarseZ);
    if (coarseIndex > m_Grid.size()) {
        //std::cout << "Coarse: x=" << coarseX << " y=" << coarseY << " z=" << coarseZ << '\n';
        //std::cout << "Global: x=" << x << " y=" << y << " z=" << z << '\n';
        return false;
    }

    if (m_Grid[coarseIndex] == EMPTY_BRICK) {
        Brick &brick = m_Bricks.emplace_back();
        m_Grid[coarseIndex] = m_Bricks.size() - 1;

        m_Textures.emplace_back();
        brick.colorPointer = m_Textures.size() - 1;
    }

    const size_t fineIndex = x % 8 + 8 * (y % 8 + 8 * (z % 8));

    Brick &brick = m_Bricks[m_Grid[coarseIndex]];
    BrickTexture &texture = m_Textures[brick.colorPointer];
    brick.Set(fineIndex, true);
    texture.voxels[fineIndex] = color;

    m_VoxelCount++;

    return true;
}

void
BrickMap::GenerateSphere() {
    ivec3 totalDimensions = m_Dimensions * 8;
    vec3 center = vec3(totalDimensions) / 2.0f;
    float radius = std::min(totalDimensions.x, std::min(totalDimensions.y, totalDimensions.z)) / 2.0f;

    float radiusSq = radius * radius;
    vec3 invDimensions = vec3(1.0f) / vec3(totalDimensions);

    for (int z = 0; z < totalDimensions.z; ++z) {
        for (int y = 0; y < totalDimensions.y; ++y) {
            for (int x = 0; x < totalDimensions.z; ++x) {
                const vec3 distanceVector = vec3(x, y, z) - center;
                const float distanceSq = dot(distanceVector, distanceVector);

                if (distanceSq < radiusSq) {
                    uint8 r = ((float) x * invDimensions.x * 255.0f);
                    uint8 g = ((float) y * invDimensions.y * 255.0f);
                    uint8 b = ((float) z * invDimensions.z * 255.0f);
                    Insert(x, y, z, {r, g, b, 255});
                }
            }
        }
    }

    std::cout << "Added " << m_VoxelCount << " voxels in " << m_Grid.size() << " bricks\n";
}

void
BrickMap::Fill() {
    const ivec3 totalDimensions = m_Dimensions * 8;
    const vec3 invDimensions = vec3(1.0f) / vec3(totalDimensions);

    siv::PerlinNoise noise;
    for (int z = 0; z < totalDimensions.z; ++z) {
        for (int y = 0; y < totalDimensions.y; ++y) {
            for (int x = 0; x < totalDimensions.z; ++x) {
                //uint8 r = ((float) x * invDimensions.x * 255.0f);
                //uint8 g = ((float) y * invDimensions.y * 255.0f);
                //uint8 b = ((float) z * invDimensions.z * 255.0f);
                constexpr float scale = 48.0f;
                const float f = noise.noise3D(
                    static_cast<float>(x) / scale,
                    static_cast<float>(y) / scale,
                    static_cast<float>(z) / scale
                );
                const uint8 u = static_cast<uint8>(f * 255.0f);
                if (f > 0.3f)
                    Insert(x, y, z, {u, u, u, 255});
            }
        }
    }
}
