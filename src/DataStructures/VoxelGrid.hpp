#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"

class VoxelGrid {
public:
  VoxelGrid();

  VoxelGrid(vec3 position, ivec3 dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  const std::vector<Color> &GetVoxels() const { return m_Voxels; }
  const AABB &GetBoundingBox() const { return m_BoundingBox; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }
  float GetVoxelSize() const { return m_VoxelSize; }

  void SetVoxels(const std::vector<Color> &voxels) {
    m_Voxels = voxels;
  }

private:
  std::vector<Color> m_Voxels;
  AABB m_BoundingBox;
  ivec3 m_Dimensions = ivec3(0);
  float m_VoxelSize = 0.0f;
  int m_VoxelCount = 0;
};
