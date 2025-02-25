#include "GraphicsNode.hpp"

#include <GL/glew.h>

#include "Camera.hpp"
#include "Model/Model.hpp"
#include "Vertex/VertexBufferLayout.hpp"

Shader GraphicsNode::s_Rasterizer;

GraphicsNode::GraphicsNode(const Mesh &mesh)
    : m_VertexBuffer(mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex)),
      m_IndexBuffer(mesh.indices.data(), mesh.indices.size()),
      m_Texture(mesh.image) {
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VertexArray.AddBuffer(m_VertexBuffer, layout);

    if (s_Rasterizer.GetProgram() == 0) {
        s_Rasterizer = std::move(Shader("shaders/rasterizer.vert", "shaders/rasterizer.frag"));
    }
}

void
GraphicsNode::Draw() const {
    s_Rasterizer.Bind();

    s_Rasterizer.SetValue("projView", Camera::GetMainCamera()->GetProjView());
    s_Rasterizer.SetValue("model", m_Transform.GetMatrix());

    m_Texture.BindTexture();

    m_VertexArray.Bind();
    m_VertexBuffer.Bind();
    m_IndexBuffer.Bind();

    glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetSize(), GL_UNSIGNED_INT, nullptr);
}
