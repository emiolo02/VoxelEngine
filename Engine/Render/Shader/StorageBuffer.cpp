#include "StorageBuffer.hpp"
#include "GL/glew.h"

void
details::CreateBuffer(uint32 &id) {
  glCreateBuffers(1, &id);
}

//------------------------------------------------------------------------------------------

void
details::Upload(const uint32 id, const size_t size, const void *data) {
  glNamedBufferStorage(id, size, data, GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT);
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
details::Destroy(const uint32 id) {
  glDeleteBuffers(1, &id);
}

//------------------------------------------------------------------------------------------

void
details::Bind(const uint32 id, const uint32 binding) {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
}
