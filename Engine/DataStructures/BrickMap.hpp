#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"
#include <cstddef>

#define EMPTY_BRICK 0xFFFFFFFF

struct Model;

class BrickMap {
public:
  struct Brick {
    Color voxels[512] = {0};
  };

  BrickMap() = default;

  BrickMap(vec3 position, ivec3 dimensions, float voxelSize);

  BrickMap(const vec3 &position, const std::vector<Color> &voxels, const ivec3 &dimensions, float voxelSize);

  BrickMap(const ivec3 &dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  const bool Insert(uint32 x, uint32 y, uint32 z, Color color);

  const std::vector<uint32> &GetGrid() const { return m_Grid; }
  const std::vector<Brick> &GetBricks() const { return m_Bricks; }
  const AABB &GetBoundingBox() const { return m_BoundingBox; }
  float GetVoxelSize() const { return m_VoxelSize; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }

  void PrintByteSize() const;

  size_t GetByteSize() const {
    return m_Grid.size() * sizeof(uint32) +
           m_Bricks.size() * sizeof(Brick);
  }

private:
  std::vector<uint32> m_Grid;
  std::vector<Brick> m_Bricks;
  AABB m_BoundingBox;
  ivec3 m_Dimensions = ivec3();
  float m_VoxelSize = 1.0f;
  int m_VoxelCount = 0;
};
