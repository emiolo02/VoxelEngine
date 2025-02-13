#include "VertexBufferLayout.hpp"
#include "GL/glew.h"

const uint32
VertexBufferLayout::Element::ByteSize() const {
  switch (type) {
    case GL_BYTE:
      return 1;
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_SHORT:
      return 2;
    case GL_UNSIGNED_SHORT:
      return 2;
    case GL_INT:
      return 4;
    case GL_UNSIGNED_INT:
      return 4;
    case GL_FLOAT:
      return 4;
    case GL_DOUBLE:
      return 8;
    default:
      return 0;
  }
}

template<typename T>
void
VertexBufferLayout::Push(uint32 count) {
  assert(false); // Unsupported type.
}

template<>
void
VertexBufferLayout::Push<int8>(uint32 count) {
  m_Elements.push_back({GL_BYTE, count});
  m_Stride += 1 * count;
}

template<>
void
VertexBufferLayout::Push<uint8>(uint32 count) {
  m_Elements.push_back({GL_UNSIGNED_BYTE, count});
  m_Stride += 1 * count;
}

template<>
void
VertexBufferLayout::Push<int16>(uint32 count) {
  m_Elements.push_back({GL_SHORT, count});
  m_Stride += 2 * count;
}

template<>
void
VertexBufferLayout::Push<uint16>(uint32 count) {
  m_Elements.push_back({GL_UNSIGNED_SHORT, count});
  m_Stride += 2 * count;
}

template<>
void
VertexBufferLayout::Push<int32>(uint32 count) {
  m_Elements.push_back({GL_INT, count});
  m_Stride += 4 * count;
}

template<>
void
VertexBufferLayout::Push<uint32>(uint32 count) {
  m_Elements.push_back({GL_UNSIGNED_INT, count});
  m_Stride += 4 * count;
}

template<>
void
VertexBufferLayout::Push<float>(uint32 count) {
  m_Elements.push_back({GL_FLOAT, count});
  m_Stride += 4 * count;
}

template<>
void
VertexBufferLayout::Push<double>(uint32 count) {
  m_Elements.push_back({GL_DOUBLE, count});
  m_Stride += 8 * count;
}
