#pragma once
#include "GL/glew.h"
#include "Material.h"
#include <vector>
struct Vertex
{
    vec3 position;
    vec2 UV;
    vec3 normal;
    vec4 tangent;

    Vertex()
    {}

    Vertex(vec3 pos, vec2 UV, vec3 normal, vec4 tangent);

    bool operator==(Vertex const& rhs);
};

struct Primitive
{
    void Draw(GLuint program);

    // Draw without textures
    void SimpleDraw();

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Material* material;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
};

struct Mesh
{
    Mesh(std::vector<Primitive> primitives)
    {
        this->primitives = primitives;
    }

    Mesh(Primitive primitive)
    {
        this->primitives.push_back(primitive);
    }

    void Draw(GLuint program)
    {
        for (auto& primitive : primitives)
            primitive.Draw(program);
    }

    // Draw without textures
    void SimpleDraw()
    {
        for (auto& primitive : primitives)
            primitive.SimpleDraw();
    }

    std::vector<Primitive> primitives;
};

struct Model
{
    Model(std::vector<Mesh> meshes);

    Model(Mesh mesh);

    Model()
    {}

    ~Model();

    void Draw(GLuint program);

    // Draw without textures
    void SimpleDraw();

    std::vector<Mesh> meshes;
};