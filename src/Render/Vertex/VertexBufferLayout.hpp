#pragma once

class VertexBufferLayout {
public:
  struct Element {
    uint32 type = 0;
    uint32 count = 0;

    const uint32 ByteSize() const;
  };

  template<typename T>
  void Push(uint32 count);

  const std::vector<Element> &GetElements() const { return m_Elements; }
  uint32 GetStride() const { return m_Stride; }

private:
  std::vector<Element> m_Elements;
  uint32 m_Stride = 0;
};
