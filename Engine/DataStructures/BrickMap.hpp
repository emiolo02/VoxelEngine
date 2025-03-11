#pragma once

#include "Math/BoundingBox.hpp"
#include "Math/Color.hpp"
#include <cstddef>

#include "Math/Ray.hpp"

#define EMPTY_BRICK 0xFFFFFFFF
#define BRICK_DIMENSIONS 8
#define BRICK_SIZE 512

struct Model;

struct VoxelHitResult {
  ivec3 position{};
  ivec3 normal{};
};

class BrickMap {
public:
  struct Brick {
    uint32 bitmask[BRICK_SIZE / 32] = {};
    uint32 colorPointer = 0;
    uint32 parent = 0;

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
    uint32 referenceCount = 0;
  };

  struct InsertResult {
    uint32 cellIndex = 0;
    bool isNew = false;
  };

  struct DeleteResult {
    uint32 cellIndex = 0;
    bool isEmpty = false;
  };

  BrickMap() = default;

  BrickMap(vec3 position, ivec3 dimensions, float voxelSize);

  BrickMap(const vec3 &position, const std::vector<math::Color> &voxels, const ivec3 &dimensions, float voxelSize);

  BrickMap(const ivec3 &dimensions, float voxelSize);

  void GenerateSphere();

  void Fill();

  std::optional<InsertResult> Insert(const ivec3 &position, math::Color color, bool replace = true);

  std::optional<InsertResult> Insert(const ivec3 &position, uint32 textureIndex);

  std::optional<DeleteResult> Delete(const ivec3 &position);

  const std::vector<uint32> &GetGrid() const { return m_Grid; }

  std::vector<uint32> &GetGrid() { return m_Grid; }

  const std::vector<Brick> &GetBricks() const { return m_Bricks; }

  std::vector<Brick> &GetBricks() { return m_Bricks; }

  const std::vector<BrickTexture> &GetBrickTextures() const { return m_Textures; }

  std::vector<BrickTexture> &GetBrickTextures() { return m_Textures; }

  const math::BoundingBox &GetBoundingBox() const { return m_BoundingBox; }
  float GetVoxelSize() const { return m_VoxelSize; }
  const ivec3 &GetDimensions() const { return m_Dimensions; }

  void PrintByteSize() const;

  std::optional<VoxelHitResult> RayCast(const math::Ray &ray);

  std::tuple<uint32 &, Brick &, BrickTexture &> GetHierarchy(const ivec3 &position);

  std::optional<math::Color> GetVoxel(const ivec3 &position) const;

private:
  std::optional<VoxelHitResult> TraverseFine(const ivec3 &brickPosition, const math::Ray &ray,
                                             const math::BoundingBox &brickBounds);

  std::vector<uint32> m_Grid;
  std::vector<Brick> m_Bricks;
  std::vector<BrickTexture> m_Textures;
  math::BoundingBox m_BoundingBox;
  ivec3 m_Dimensions = ivec3();
  float m_VoxelSize = 1.0f;
  int m_VoxelCount = 0;
};

std::string PrefixedSize(float size);
