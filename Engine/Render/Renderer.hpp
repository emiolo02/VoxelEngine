#pragma once

#include "GraphicsNode.hpp"
#include "Model/Model.hpp"

#include "Shader/Shader.hpp"

class Renderer {
public:
    Renderer();

    void Draw();

    void AddMesh(const Mesh &mesh);

    void AddModel(const Model &model);

private:
    std::vector<GraphicsNode> m_Scene;
    Shader m_Shader;
};
