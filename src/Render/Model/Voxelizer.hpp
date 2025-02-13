#pragma once

#include <DataStructures/VoxelGrid.hpp>

#include "Model.hpp"
#include "Math/Color.hpp"

struct Model;
struct Mesh;

class BrickMap;

struct BoundingBox {
    vec3 min = {}, max = {};

    BoundingBox() = default;

    BoundingBox(const vec3 &min, const vec3 &max)
        : min(min), max(max) {
    }

    // Assuming size is positive
    BoundingBox(const vec3 &center, const float size)
        : min(center - size), max(center + size) {
    }

    void SetSize(const vec3 &newSize) {
        const vec3 c = GetCenter();
        min = c - newSize;
        max = c + newSize;
    }

    vec3 GetCenter() const {
        return (min + max) * 0.5f;
    }

    vec3 GetSize() const {
        return max - min;
    }
};

struct Triangle {
    Vertex a, b, c;
    uint32 imageId;

    vec3 GetNormal() const {
        return cross(b.position - a.position, c.position - a.position);
    }

    bool PointInTriangle(const vec3 &query_point,
                         float *b0 = nullptr,
                         float *b1 = nullptr,
                         float *b2 = nullptr) const {
        // u=P2−P1
        const vec3 u = b.position - a.position;
        // v=P3−P1
        const vec3 v = c.position - a.position;
        // n=u×v
        const vec3 n = cross(u, v);
        // w=P−P1
        const vec3 w = query_point - a.position;
        // Barycentric coordinates of the projection P′of P onto T:
        // γ=[(u×w)⋅n]/n²
        const float gamma = dot(cross(u, w), n) / dot(n, n);
        // β=[(w×v)⋅n]/n²
        const float beta = dot(cross(w, v), n) / dot(n, n);
        const float alpha = 1.0f - gamma - beta;

        if (b0) *b0 = alpha;
        if (b1) *b1 = beta;
        if (b2) *b2 = gamma;

        constexpr float p = 1e-6f;
        // The point P′ lies inside T if:
        return ((-p <= alpha) && (alpha <= 1.0f + p) &&
                (-p <= beta) && (beta <= 1.0f + p) &&
                (-p <= gamma) && (gamma <= 1.0f + p));
    }

    const Vertex &operator[](const size_t i) const {
        switch (i) {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            default: return {};
        }
    }
};

class OctreeMesh {
public:
    struct Node {
        Node() = default;

        Node(const OctreeMesh *octree, Node *parent)
            : octree(octree), parent(parent) {
        }

        ~Node() {
            delete[] children;
        }

        void Draw();

        //~Node() {
        //    for (int i = 0; i < 8; ++i)
        //        delete children[i];
        //}

        const OctreeMesh *octree = nullptr;
        Node *parent = nullptr;
        Node *children = nullptr;
        bool isLeaf = false;
        bool isFilled = false;
        BoundingBox boundingBox;
        Color color = {};

        std::vector<Triangle> triangles;

        void Subdivide(uint32 subdivisions);

        void Linearize(uint32 level, const ivec3 &globalPosition, std::vector<Color> &arr);

        void BrickMapify(uint32 level, const ivec3 &globalPosition, BrickMap &bm);

        void Clear();
    };

    explicit OctreeMesh(const Mesh &mesh);

    explicit OctreeMesh(const Model &model);

    void Subdivide(uint32 subdivisions);

    void Clear();

    void Draw();

    std::vector<Color> Linearize();

    BrickMap CreateBrickMap(float voxelSize);

    VoxelGrid CreateVoxelGrid(float voxelSize);

    uint32 GetSize() const;

private:
    Node m_Root;
    uint32 m_Size = 0;
};

std::pair<std::vector<Color>, ivec3> Voxelize(uint32 subdivisions, const Model &model);
