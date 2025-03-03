#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"
#include <cstddef>

#include "Math/Ray.hpp"

#define EMPTY_BRICK 0xFFFFFFFF
#define BRICK_DIMENSIONS 8
#define BRICK_SIZE 512

struct Model;

class BrickMap {
public:
  struct Brick {
    uint32 bitmask[BRICK_SIZE / 32] = {};
    uint32 colorPointer = 0;

    void Set(const uint32 bit, const bool value) {
      assert(bit < BRICK_SIZE);

      const uint32 index = bit / 32;

      if (value) {
        bitmask[index] = bitmask[index] | static_cast<uint32>(1) << bit % 32;
      } else {
        bitmask[index] = bitmask[index] & ~(static_cast<uint32>(1) << bit % 32);
      }
    }

    bool VoxelAt(const uint32 bit) const {
      const uint32 index = bit / 32;
      return bitmask[index] >> (bit % 32) & 1;
    }
  };

  struct BrickTexture {
    math::Color voxels[BRICK_SIZE] = {};
  };

  BrickMap() = default;

  BrickMap(vec3 position, ivec3 dimensions, float voxelSize);

  BrickMap(const vec3 &position, const std::vector<math::Color> &voxels, const ivec3 &dimensions, float voxelSize);

  BrickMap(const ivec3 &dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  bool Insert(uint32 x, uint32 y, uint32 z, math::Color color);

  const std::vector<uint32> &GetGrid() const { return m_Grid; }
  const std::vector<Brick> &GetBricks() const { return m_Bricks; }
  const std::vector<BrickTexture> &GetBrickTextures() const { return m_Textures; }
  const math::BoundingBox &GetBoundingBox() const { return m_BoundingBox; }
  float GetVoxelSize() const { return m_VoxelSize; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }

  void PrintByteSize() const;

  // Returns position of hit voxel.
  std::optional<ivec3> RayCast(const math::Ray &ray);

  std::tuple<uint32 &, Brick &, BrickTexture &> GetHierarchy(const ivec3 &position);

  std::vector<math::BoundingBox> hitGridCells;

private:
  std::optional<ivec3> TraverseCoarse(const math::Ray &ray);

  std::optional<ivec3> TraverseFine(const ivec3 &brickPosition, const math::Ray &ray,
                                    const math::BoundingBox &brickBounds);

  std::vector<uint32> m_Grid;
  std::vector<Brick> m_Bricks;
  std::vector<BrickTexture> m_Textures;
  math::BoundingBox m_BoundingBox;
  ivec3 m_Dimensions = ivec3();
  float m_VoxelSize = 1.0f;
  int m_VoxelCount = 0;
};
