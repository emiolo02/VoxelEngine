#include "IndexBuffer.hpp"
#include <glad/glad.h>

IndexBuffer::IndexBuffer(const uint32 *data, size_t size)
  : m_Size(size) {
  glGenBuffers(1, &m_Id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * 4, data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
  glDeleteBuffers(1, &m_Id);
}

void
IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void
IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
