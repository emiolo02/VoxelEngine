#pragma once

class VertexBuffer {
public:
  // Byte size of entire buffer.
  VertexBuffer(const void *data, size_t byteSize);

  ~VertexBuffer();

  void Bind() const;

  void Unbind() const;

private:
  uint32 m_Id;
};
