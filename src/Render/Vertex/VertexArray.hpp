#pragma once
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

class VertexBuffer;
class VertexBufferLayout;

class VertexArray {
public:
    VertexArray();

    ~VertexArray();

    void Bind() const;

    void Unbind() const;

    void AddBuffer(const VertexBuffer &vertexBuffer,
                   const VertexBufferLayout &layout);

private:
    uint32 m_Id = 0;
};
