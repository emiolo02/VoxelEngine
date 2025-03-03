#include "VertexArray.hpp"
#include <glad/glad.h>

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_Id);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_Id);
}

void
VertexArray::Bind() const {
  glBindVertexArray(m_Id);
}

void
VertexArray::Unbind() const {
  glBindVertexArray(0);
}

void
VertexArray::AddBuffer(const VertexBuffer &vertexBuffer,
                       const VertexBufferLayout &layout) {
  Bind();
  vertexBuffer.Bind();
  const auto &layoutElements = layout.GetElements();

  size_t offset = 0;
  for (uint32 i = 0; i < layoutElements.size(); ++i) {
    const auto &element = layoutElements[i];

    glEnableVertexAttribArray(i);
    glVertexAttribPointer(
      i, element.count, element.type, false, layout.GetStride(), (void *) offset);

    offset += element.count * element.ByteSize();
  }
}
