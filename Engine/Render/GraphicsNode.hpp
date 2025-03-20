#pragma once

#include "Shader/Shader.hpp"

#include "Vertex/VertexArray.hpp"
#include "Vertex/VertexBuffer.hpp"
#include "Vertex/IndexBuffer.hpp"

#include "Utility/Transform.hpp"

#include "Model/Model.hpp"

class GraphicsNode {
public:
    GraphicsNode() = delete;

    explicit GraphicsNode(const Mesh &mesh);

    void Draw() const;

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

    void SetPosition(const vec3 &position) { m_Transform.position = position; }
    void SetScale(const vec3 &scale) { m_Transform.scale = scale; }
    void SetOrientation(const vec3 &orientation) { m_Transform.orientation = orientation; }

private:
    VertexArray m_VertexArray;
    VertexBuffer m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    Material m_Material;

    Transform m_Transform;

    static Shader s_Rasterizer;
};
