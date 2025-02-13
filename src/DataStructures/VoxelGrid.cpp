#include "VoxelGrid.hpp"
#include "glm/ext/vector_float3.hpp"

VoxelGrid::VoxelGrid()
  : m_Voxels({0xFFFFFFFF})
    , m_Dimensions(1, 1, 1)
    , m_VoxelSize(1.0f)
    , m_VoxelCount(1) {
}

VoxelGrid::VoxelGrid(vec3 position, ivec3 dimensions, float voxelSize)
  : m_Dimensions(dimensions)
    , m_VoxelSize(voxelSize) {
  vec3 boundSize = vec3(dimensions) * voxelSize;
  m_BoundingBox = AABB(position, boundSize + position);

  m_Voxels.resize(m_Dimensions.x * m_Dimensions.y * m_Dimensions.z);
  const size_t byteSize = m_Voxels.size() * sizeof(Color);

  if (byteSize > 1000000000)
    std::cout << "Size of voxel grid: " << byteSize / 1000000000.0f << " gb\n";
  else if (byteSize > 1000000)
    std::cout << "Size of voxel grid: " << byteSize / 1000000.0f << " mb\n";
  else if (byteSize > 1000)
    std::cout << "Size of voxel grid: " << byteSize / 1000.0f << " kb\n";
  else
    std::cout << "Size of voxel grid: " << byteSize << " b\n";
}

void
VoxelGrid::GenerateSphere() {
  vec3 center = vec3(m_Dimensions) / 2.0f;
  float radius =
      std::min(m_Dimensions.x, std::min(m_Dimensions.y, m_Dimensions.z)) / 2.0f;

  float radiusSq = radius * radius;
  vec3 invDimensions = vec3(1.0f) / vec3(m_Dimensions);

  for (int z = 0; z < m_Dimensions.z; ++z) {
    for (int y = 0; y < m_Dimensions.y; ++y) {
      for (int x = 0; x < m_Dimensions.z; ++x) {
        const vec3 distanceVector = vec3(x, y, z) - center;
        const float distanceSq = dot(distanceVector, distanceVector);
        if (distanceSq < radiusSq) {
          uint8 r = ((float) x * invDimensions.x * 255.0f);
          uint8 g = ((float) y * invDimensions.y * 255.0f);
          uint8 b = ((float) z * invDimensions.z * 255.0f);

          const int index = x + m_Dimensions.x * (y + m_Dimensions.y * z);

          m_Voxels[index] = {
            r, g, b, 0xFF
          };

          m_VoxelCount++;
        }
      }
    }
  }

  std::cout << "Added " << m_VoxelCount << " voxels\n";
}

void
VoxelGrid::Fill() {
  vec3 invDimensions = vec3(1.0f) / vec3(m_Dimensions);
  for (int z = 0; z < m_Dimensions.z; ++z) {
    for (int y = 0; y < m_Dimensions.y; ++y) {
      for (int x = 0; x < m_Dimensions.z; ++x) {
        uint8 r = ((float) x * invDimensions.x * 255.0f);
        uint8 g = ((float) y * invDimensions.y * 255.0f);
        uint8 b = ((float) z * invDimensions.z * 255.0f);
        const int index = x + m_Dimensions.x * (y + m_Dimensions.y * z);

        m_Voxels[index] = {
          r, g, b, 0xFF
        };
      }
    }
  }
}
