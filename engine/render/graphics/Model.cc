#include "config.h"
#include "Model.h"

Vertex::Vertex(vec3 pos, vec2 UV, vec3 normal, vec4 tangent) :
    position(pos), UV(UV), normal(normal), tangent(tangent)
{}

bool 
Vertex::operator==(Vertex const& rhs)
{
    return (this->position == rhs.position &&
        this->UV == rhs.UV &&
        this->normal == rhs.normal);
}

void 
Primitive::Draw(GLuint program)
{
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

    material->Apply(program);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}

// Draw without textures
void 
Primitive::SimpleDraw()
{
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}

Model::Model(std::vector<Mesh> meshes)
{
    this->meshes = meshes;
}

Model::Model(Mesh mesh)
{
    this->meshes.push_back(mesh);
}

Model::~Model()
{
    for (auto& mesh : meshes)
    {
        for (auto& prim : mesh.primitives)
        {
            delete prim.material;
        }
    }
}

void 
Model::Draw(GLuint program)
{
    for (auto& mesh : meshes)
        mesh.Draw(program);
}

// Draw without textures
void 
Model::SimpleDraw()
{
    for (auto& mesh : meshes)
        mesh.SimpleDraw();
}