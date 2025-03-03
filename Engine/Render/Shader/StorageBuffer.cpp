#include "StorageBuffer.hpp"
#include <glad/glad.h>

void
details::CreateBuffer(uint32 &id) {
  glCreateBuffers(1, &id);
}

//------------------------------------------------------------------------------------------

void
details::Upload(const uint32 id, const size_t size, const void *data) {
  glNamedBufferStorage(id, size, data, GL_DYNAMIC_STORAGE_BIT);
}

//------------------------------------------------------------------------------------------

void
details::GetData(const uint32 id, const size_t offset, const size_t size, void *data) {
  glGetNamedBufferSubData(id, offset, size, data);
}

//------------------------------------------------------------------------------------------

void
details::SetData(const uint32 id, const size_t offset, const size_t size, const void *data) {
  glNamedBufferSubData(id, offset, size, data);
}

//------------------------------------------------------------------------------------------

void
details::SetElement(const uint32 id, const size_t index, const size_t size, const void *element) {
  glNamedBufferSubData(id, index, size, element);
}

//------------------------------------------------------------------------------------------

void
details::Clear(const uint32 id) {
  glDeleteBuffers(1, &id);
}

//------------------------------------------------------------------------------------------

void
details::Bind(const uint32 id, const uint32 binding) {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
}
