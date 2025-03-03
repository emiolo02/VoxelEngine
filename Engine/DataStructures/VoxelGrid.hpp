#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"

class VoxelGrid {
public:
  VoxelGrid();

  VoxelGrid(vec3 position, ivec3 dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  const std::vector<math::Color> &GetVoxels() const { return m_Voxels; }
  const math::BoundingBox &GetBoundingBox() const { return m_BoundingBox; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }
  float GetVoxelSize() const { return m_VoxelSize; }

  void SetVoxels(const std::vector<math::Color> &voxels) {
    m_Voxels = voxels;
  }

private:
  std::vector<math::Color> m_Voxels;
  math::BoundingBox m_BoundingBox;
  ivec3 m_Dimensions = ivec3(0);
  float m_VoxelSize = 0.0f;
  int m_VoxelCount = 0;
};
