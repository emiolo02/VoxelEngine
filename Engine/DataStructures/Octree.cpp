#include "Octree.hpp"

Octree::Node::Node(uint64 childPointer, uint16 colorPointer) {
  m_Descriptor = (childPointer << 16) + colorPointer;
}
