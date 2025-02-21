#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"
#include <cstddef>

#define EMPTY_BRICK 0xFFFFFFFF
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
    Color voxels[BRICK_SIZE] = {};
  };

  BrickMap() = default;

  BrickMap(vec3 position, ivec3 dimensions, float voxelSize);

  BrickMap(const vec3 &position, const std::vector<Color> &voxels, const ivec3 &dimensions, float voxelSize);

  BrickMap(const ivec3 &dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  bool Insert(uint32 x, uint32 y, uint32 z, Color color);

  const std::vector<uint32> &GetGrid() const { return m_Grid; }
  const std::vector<Brick> &GetBricks() const { return m_Bricks; }
  const std::vector<BrickTexture> &GetBrickTextures() const { return m_Textures; }
  const BoundingBox &GetBoundingBox() const { return m_BoundingBox; }
  float GetVoxelSize() const { return m_VoxelSize; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }

  void PrintByteSize() const;

  size_t GetByteSize() const {
    return m_Grid.size() * sizeof(uint32) +
           m_Bricks.size() * sizeof(Brick) +
           m_Textures.size() * sizeof(BrickTexture);
  }

private:
  std::vector<uint32> m_Grid;
  std::vector<Brick> m_Bricks;
  std::vector<BrickTexture> m_Textures;
  BoundingBox m_BoundingBox;
  ivec3 m_Dimensions = ivec3();
  float m_VoxelSize = 1.0f;
  int m_VoxelCount = 0;
};
