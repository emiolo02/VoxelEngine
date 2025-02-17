#pragma once

class SSBO {
public:
  // Byte size of entire buffer.
  SSBO(const void *data, size_t byteSize, uint32 binding);

  ~SSBO();

  void Bind() const;

  void UnBind() const;

  void Replace(size_t index, void *data, size_t byteSize);

  size_t GetSize() const { return m_Size; }

private:
  uint32 m_Id = 0;
  uint32 m_Binding = 0;
  size_t m_Size = 0;
};
