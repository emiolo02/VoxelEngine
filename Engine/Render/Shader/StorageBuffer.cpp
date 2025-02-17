#include "StorageBuffer.hpp"
#include "GL/glew.h"

SSBO::SSBO(const void *data, size_t byteSize, uint32 binding)
  : m_Size(byteSize)
    , m_Binding(binding) {
  glCreateBuffers(1, &m_Id);

  glNamedBufferStorage(m_Id, m_Size, data, GL_DYNAMIC_STORAGE_BIT);
}

SSBO::~SSBO() {
  glDeleteBuffers(1, &m_Id);
}

void
SSBO::Bind() const {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_Id);
}

void
SSBO::UnBind() const {
  std::cout << "SSBO::UnBind is not implemented.\n";
}

void
SSBO::Replace(size_t index, void *data, size_t byteSize) {
  glNamedBufferSubData(m_Id, index, byteSize, data);
}
