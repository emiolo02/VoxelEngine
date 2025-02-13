#include "BrickMap.hpp"
#include "glm/ext/vector_int3.hpp"
#include <cmath>

#include "Math/PerlinNoise.hpp"

BrickMap::BrickMap(vec3 position, ivec3 dimensions, float voxelSize)
  : m_VoxelSize(voxelSize) {
  vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
  m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                       std::ceil(fCoarseDimensions.y),
                       std::ceil(fCoarseDimensions.z));

  m_BoundingBox = AABB(position, vec3(m_Dimensions) * 8.0f * voxelSize + position);

  m_Grid.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z, EMPTY_BRICK);
}

BrickMap::BrickMap(const vec3 &position, const std::vector<Color> &voxels, const ivec3 &dimensions,
                   const float voxelSize)
  : m_VoxelSize(voxelSize) {
  vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
  m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                       std::ceil(fCoarseDimensions.y),
                       std::ceil(fCoarseDimensions.z));

  m_BoundingBox = AABB(position, vec3(m_Dimensions) * 8.0f * voxelSize + position);

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

BrickMap::BrickMap(const ivec3 &dimensions, float voxelSize)
  : m_VoxelSize(voxelSize) {
  vec3 fCoarseDimensions = vec3(dimensions) / 8.0f;
  m_Dimensions = ivec3(std::ceil(fCoarseDimensions.x),
                       std::ceil(fCoarseDimensions.y),
                       std::ceil(fCoarseDimensions.z));

  m_BoundingBox = AABB(vec3(0.0f), vec3(m_Dimensions) * 8.0f * voxelSize);

  m_Grid.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z, EMPTY_BRICK);
}


void
BrickMap::PrintByteSize() const {
  size_t byteSize = GetByteSize();
  if (byteSize > 1000000000)
    std::cout << "Size of brick map: " << byteSize / 1000000000.0f << " gb\n";
  else if (byteSize > 1000000)
    std::cout << "Size of brick map: " << byteSize / 1000000.0f << " mb\n";
  else if (byteSize > 1000)
    std::cout << "Size of brick map: " << byteSize / 1000.0f << " kb\n";
  else
    std::cout << "Size of brick map: " << byteSize << " b\n";
}

const bool
BrickMap::Insert(uint32 x, uint32 y, uint32 z, Color color) {
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
    m_Bricks.push_back(Brick());
    m_Grid[coarseIndex] = m_Bricks.size() - 1;
  }

  const size_t fineIndex = x % 8 + 8 * (y % 8 + 8 * (z % 8));

  Brick &brick = m_Bricks[m_Grid[coarseIndex]];
  brick.voxels[fineIndex] = color;

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
