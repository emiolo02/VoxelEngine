#include "Renderer.hpp"

Renderer::Renderer()
    : m_Shader("shaders/TexturedObject.vert", "shaders/TexturedObject.frag") {
}

void
Renderer::Draw() {
    for (const auto &node: m_Scene) {
    }
}

void
Renderer::AddMesh(const Mesh &mesh) {
    m_Scene.emplace_back(mesh);
}

void
Renderer::AddModel(const Model &model) {
    for (const auto &mesh: model.meshes) {
        m_Scene.emplace_back(mesh);
    }
}
