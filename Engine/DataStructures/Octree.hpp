#pragma once

#include "Math/Color.hpp"

class Octree {
public:
  class Node {
  public:
    Node(uint64 descriptor)
      : m_Descriptor(descriptor) {
    }

    Node(uint64 childPointer, uint16 colorPointer);

    inline const uint64 GetChildPointer() { return m_Descriptor >> 16; }
    inline const uint16 GetColorPointer() { return m_Descriptor & 0xFFFF; }

  private:
    uint64 m_Descriptor = 0;
  };

private:
  std::vector<Node> m_Voxels = {0};
  std::vector<Color> m_Colors = {0};
};
