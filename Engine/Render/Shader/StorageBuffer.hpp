#pragma once

namespace details {
    void CreateBuffer(uint32 &id);

    void Upload(uint32 id, size_t size, const void *data);

    void GetData(uint32 id, size_t offset, size_t size, void *data);

    void SetData(uint32 id, size_t offset, size_t size, const void *data);

    void Destroy(uint32 id);

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

    void SetData(size_t offset, const std::vector<T> &data);

    void SetData(size_t index, const T &element);

    void PushBack(const T &element);

    void PopBack() { m_Size--; }

    void Reserve(size_t newCapacity);

    size_t GetSize() const { return m_Size; }
    size_t GetCapacity() const { return m_Capacity; }
    bool Empty() const { return m_Size == 0; }

private:
    uint32 m_Id = 0;
    uint32 m_Binding = 0;

    size_t m_Size = 0;
    size_t m_Capacity = 0;
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
    : m_Binding(binding), m_Size(data.size()), m_Capacity(data.size()) {
    details::CreateBuffer(m_Id);
    details::Upload(m_Id, data.size() * sizeof(T), data.data());
}

//------------------------------------------------------------------------------------------

template<typename T>
StorageBuffer<T>::~StorageBuffer() {
    details::Destroy(m_Id);
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::Upload(const std::vector<T> &data) {
    m_Size = data.size();
    m_Capacity = data.size();
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

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::SetData(const size_t offset, const std::vector<T> &data) {
    details::SetData(m_Id, offset * sizeof(T), data.size() * sizeof(T), data.data());
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::SetData(const size_t index, const T &element) {
    details::SetData(m_Id, index * sizeof(T), sizeof(T), &element);
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::PushBack(const T &element) {
    if (m_Size == m_Capacity) {
        // Growth factor of 1.25
        Reserve((5 * m_Size) >> 2);
    }

    details::SetData(m_Id, m_Size * sizeof(T), sizeof(T), &element);
    m_Size++;
}

//------------------------------------------------------------------------------------------

template<typename T>
void
StorageBuffer<T>::Reserve(const size_t newCapacity) {
    if (newCapacity <= m_Capacity) return;

    m_Capacity = newCapacity;

    std::vector<T> data(m_Size);

    details::GetData(m_Id, 0, m_Size * sizeof(T), data.data());

    details::Destroy(m_Id);
    details::CreateBuffer(m_Id);
    details::Upload(m_Id, m_Capacity * sizeof(T), data.data());
}
