#pragma once

class IndexBuffer
{
public:
  // Size is number of elements.
  IndexBuffer(const uint32* data, size_t size);
  ~IndexBuffer();

  void Bind() const;
  void Unbind() const;

  inline const size_t GetSize() const { return m_Size; }

private:
  uint32 m_Id;
  size_t m_Size;
};
