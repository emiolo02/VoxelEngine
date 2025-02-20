#pragma once

namespace details {
  void CreateBuffer(uint32 &);

  void Upload(uint32, size_t, const void *);

  void GetData(uint32, size_t, size_t, void *);

  void Clear(uint32);

  void Bind(uint32, uint32);
}

//------------------------------------------------------------------------------------------

template<typename T>
class StorageBuffer {
public:
  explicit StorageBuffer(uint32 binding);

  StorageBuffer(const std::vector<T> &data, uint32 binding);

  ~StorageBuffer();

  void Upload(const std::vector<T> &data);

  void Bind() const;

  std::vector<T> GetData(size_t offset, size_t num) const;

  size_t GetSize() const { return m_Size; }

private:
  uint32 m_Id = 0;
  uint32 m_Binding = 0;
  size_t m_Size = 0;
};

//------------------------------------------------------------------------------------------

template<typename T>
StorageBuffer<T>::StorageBuffer(const uint32 binding)
  : m_Binding(binding) {
  details::CreateBuffer(m_Id);
}

//------------------------------------------------------------------------------------------

template<typename T>
StorageBuffer<T>::StorageBuffer(const std::vector<T> &data, const uint32 binding)
  : m_Binding(binding), m_Size(data.size()) {
  details::CreateBuffer(m_Id);
  details::Upload(m_Id, data.size() * sizeof(T), data.data());
}

//------------------------------------------------------------------------------------------

template<typename T>
StorageBuffer<T>::~StorageBuffer() {
  details::Clear(m_Id);
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::Upload(const std::vector<T> &data) {
  details::Upload(m_Id, data.size() * sizeof(T), data.data());
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::Bind() const {
  details::Bind(m_Id, m_Binding);
}

//------------------------------------------------------------------------------------------

template<typename T>
std::vector<T>
StorageBuffer<T>::GetData(const size_t offset, const size_t num) const {
  const size_t clamped = offset + num > m_Size ? m_Size - offset : num;
  std::vector<T> data(clamped);
  details::GetData(m_Id, offset, clamped * sizeof(T), data.data());
  return data;
}
