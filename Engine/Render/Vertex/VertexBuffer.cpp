#include "VertexBuffer.hpp"

#include <glad/glad.h>
#include <cstring>

VertexBuffer::VertexBuffer(const void *data, const size_t byteSize) {
  glGenBuffers(1, &m_Id);
  glBindBuffer(GL_ARRAY_BUFFER, m_Id);
  glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &m_Id);
}

void
VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void
VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
