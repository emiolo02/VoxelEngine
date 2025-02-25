#pragma once

struct Image;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;

    bool operator==(const Vertex &rhs) const {
        return position == rhs.position && uv == rhs.uv;
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    const Image &image;
};

struct Model {
    std::vector<Mesh> meshes;
};

