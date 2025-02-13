#include "GraphicsNode.hpp"

#include "Model/Model.hpp"
#include "Vertex/VertexBufferLayout.hpp"

GraphicsNode::GraphicsNode(const Mesh &mesh)
    : m_VertexBuffer(mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex)),
      m_IndexBuffer(mesh.indices.data(), mesh.indices.size() * sizeof(uint32)),
      m_Texture(mesh.image) {
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VertexArray.AddBuffer(m_VertexBuffer, layout);
}

void
GraphicsNode::Bind() const {
}
