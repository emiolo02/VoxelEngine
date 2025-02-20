#include "Voxelizer.hpp"
#include "Model.hpp"
#include "DataStructures/BrickMap.hpp"
#include "Render/Debug.hpp"
#include <thread>
#include <Render/Texture/Image.hpp>

bool
Intersect(const BoundingBox &box, const Triangle &tri) {
    // Compute the triangle edges
    vec3 e0 = tri.b.position - tri.a.position;
    vec3 e1 = tri.c.position - tri.b.position;
    vec3 e2 = tri.a.position - tri.c.position;

    // Compute the triangle normal
    vec3 normal = cross(e0, e1);

    // Get the AABB center and extents
    vec3 boxCenter = {
        (box.min[0] + box.max[0]) * 0.5f,
        (box.min[1] + box.max[1]) * 0.5f,
        (box.min[2] + box.max[2]) * 0.5f
    };

    vec3 boxHalfSize = {
        (box.max[0] - box.min[0]) * 0.5f,
        (box.max[1] - box.min[1]) * 0.5f,
        (box.max[2] - box.min[2]) * 0.5f
    };

    // Move triangle to AABB's local space
    vec3 a_local = tri.a.position - boxCenter;
    vec3 b_local = tri.b.position - boxCenter;
    vec3 c_local = tri.c.position - boxCenter;

    // Test the 3 AABB axes (x, y, z)
    for (int i = 0; i < 3; i++) {
        float triMin = std::min({a_local[i], b_local[i], c_local[i]});
        float triMax = std::max({a_local[i], b_local[i], c_local[i]});
        float boxMin = -boxHalfSize[i];
        float boxMax = boxHalfSize[i];
        // Test if two projected intervals overlap
        if (std::max(triMin, boxMin) > std::min(triMax, boxMax)) {
            return false; // Separating axis found
        }
    }

    // Test the triangle normal as a separating axis
    float p0 = dot(a_local, normal);
    float p1 = dot(b_local, normal);
    float p2 = dot(c_local, normal);
    float triMin = std::min({p0, p1, p2});
    float triMax = std::max({p0, p1, p2});

    float boxRadius = boxHalfSize[0] * std::fabs(normal[0]) +
                      boxHalfSize[1] * std::fabs(normal[1]) +
                      boxHalfSize[2] * std::fabs(normal[2]);

    if (triMin > boxRadius || triMax < -boxRadius) {
        return false; // Separating axis found
    }

    // Test the 9 cross-product axes of the triangle edges and the box axes
    vec3 axes[] = {
        {0, -e0[2], e0[1]}, {0, -e1[2], e1[1]}, {0, -e2[2], e2[1]},
        {e0[2], 0, -e0[0]}, {e1[2], 0, -e1[0]}, {e2[2], 0, -e2[0]},
        {-e0[1], e0[0], 0}, {-e1[1], e1[0], 0}, {-e2[1], e2[0], 0}
    };

    for (const vec3 &axis: axes) {
        float p0 = dot(a_local, axis);
        float p1 = dot(b_local, axis);
        float p2 = dot(c_local, axis);
        float triMin = std::min({p0, p1, p2});
        float triMax = std::max({p0, p1, p2});

        float boxRadius = boxHalfSize[0] * std::fabs(axis[0]) +
                          boxHalfSize[1] * std::fabs(axis[1]) +
                          boxHalfSize[2] * std::fabs(axis[2]);

        if (triMin > boxRadius || triMax < -boxRadius) {
            return false; // Separating axis found
        }
    }

    // No separating axis found, so the triangle and AABB intersect
    return true;
}


OctreeMesh::OctreeMesh(const Mesh &mesh)
    : m_Root(this, nullptr) {
    std::vector<Triangle> triangles(mesh.indices.size() / 3);

    BoundingBox rootBounds = {vec3(FLT_MAX), vec3(-FLT_MAX)};

    const uint32 imageId = mesh.image.id;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        const Triangle &triangle = triangles[i / 3] = {
                                       mesh.vertices[mesh.indices[i]],
                                       mesh.vertices[mesh.indices[i + 1]],
                                       mesh.vertices[mesh.indices[i + 2]],
                                       imageId
                                   };

        for (int j = 0; j < 3; ++j) {
            rootBounds.min = vec3(
                std::min(rootBounds.min.x, triangle[j].position.x),
                std::min(rootBounds.min.y, triangle[j].position.y),
                std::min(rootBounds.min.z, triangle[j].position.z)
            );
            rootBounds.max = vec3(
                std::max(rootBounds.max.x, triangle[j].position.x),
                std::max(rootBounds.max.y, triangle[j].position.y),
                std::max(rootBounds.max.z, triangle[j].position.z)
            );
        }
    }

    // Expand bounds so it is equal on all sides.
    const vec3 boundSize = rootBounds.GetSize();
    rootBounds.SetSize(vec3(std::max(std::max(boundSize.x, boundSize.y), boundSize.z)));

    m_Root.boundingBox = rootBounds;
    m_Root.triangles = triangles;
}

OctreeMesh::OctreeMesh(const Model &model)
    : m_Root(this, nullptr) {
    std::vector<Triangle> triangles;
    BoundingBox rootBounds = {vec3(FLT_MAX), vec3(-FLT_MAX)};

    for (auto &mesh: model.meshes) {
        const uint32 imageId = mesh.image.id;

        for (unsigned i = 0; i < mesh.indices.size(); i += 3) {
            const Triangle &triangle = triangles.emplace_back(
                mesh.vertices[mesh.indices[i]],
                mesh.vertices[mesh.indices[i + 1]],
                mesh.vertices[mesh.indices[i + 2]],
                imageId
            );

            for (unsigned j = 0; j < 3; ++j) {
                rootBounds.min = {
                    std::min(rootBounds.min.x, triangle[j].position.x),
                    std::min(rootBounds.min.y, triangle[j].position.y),
                    std::min(rootBounds.min.z, triangle[j].position.z)
                };
                rootBounds.max = {
                    std::max(rootBounds.max.x, triangle[j].position.x),
                    std::max(rootBounds.max.y, triangle[j].position.y),
                    std::max(rootBounds.max.z, triangle[j].position.z)
                };
            }
        }
    }

    triangles.shrink_to_fit();
    // Expand bounds so it is equal on all sides.
    const vec3 boundSize = rootBounds.GetSize();
    rootBounds.SetSize(vec3(std::max(std::max(boundSize.x, boundSize.y), boundSize.z)));

    m_Root.boundingBox = rootBounds;
    m_Root.triangles = triangles;
}

void
OctreeMesh::Subdivide(const uint32 subdivisions) {
    std::cout << "Subdividing mesh.\n";
    m_Size = std::exp2(subdivisions);
    m_Root.Subdivide(subdivisions);

    std::cout << "Octree dimensions: " << m_Size << '\n';
}

void
OctreeMesh::Clear() {
    m_Root.Clear();
}

static uint32 sentDrawCalls = 0;

void OctreeMesh::Draw() {
    sentDrawCalls = 0;
    m_Root.Draw();

    std::cout << "Sent draw calls: " << sentDrawCalls << '\n';
}

std::vector<Color>
OctreeMesh::Linearize() {
    std::vector<Color> voxels(m_Size * m_Size * m_Size);
    m_Root.Linearize(0, ivec3(0), voxels);
    return voxels;
}

BrickMap
OctreeMesh::CreateBrickMap(const float voxelSize) {
    std::cout << "Creating brickmap.\n";
    BrickMap brickMap(ivec3(m_Size), voxelSize);
    m_Root.BrickMapify(0, ivec3(0), brickMap);
    return brickMap;
}

VoxelGrid
OctreeMesh::CreateVoxelGrid(const float voxelSize) {
    std::cout << "Creating voxel grid.\n";
    std::vector<Color> voxels(m_Size * m_Size * m_Size);
    m_Root.Linearize(0, ivec3(0), voxels);
    VoxelGrid grid(vec3(0.0f), ivec3(m_Size), voxelSize);
    grid.SetVoxels(voxels);
    return grid;
}

uint32
OctreeMesh::GetSize() const {
    return m_Size;
}


void
OctreeMesh::Node::Draw() {
    if (isLeaf || parent == nullptr) {
        Debug::DrawBox(boundingBox.GetCenter(), {}, vec3(boundingBox.GetSize() / 2.0f), vec4(0, 1, 0, 1), 2);
        sentDrawCalls++;
    }
    if (isLeaf) return;

    for (int i = 0; i < 8; ++i) {
        children[i].Draw();
    }
}

void
OctreeMesh::Node::Subdivide(const uint32 subdivisions) {
    //std::cout << triangles.size() << '\n';
    if (subdivisions == 0) {
        isLeaf = true;

        for (const auto &triangle: triangles) {
            if (Intersect(boundingBox, triangle)) {
                isFilled = true;
                break;
            }
        }

        const vec3 center = boundingBox.GetCenter();
        vec2 uv;
        for (const auto &triangle: triangles) {
            float b0, b1, b2;
            if (triangle.PointInTriangle(center, &b0, &b1, &b2)) {
                uv = {
                    b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x,
                    b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y
                };
                break;
            }
            assert(!std::isnan(uv.x) && !std::isnan(uv.y));
        }
        const Image &image = ImageManager::Get().GetImage(triangles[0].imageId);
        const int imagex = std::fmod(uv.x, 1.0f) * image.width;
        const int imagey = std::fmod(uv.y, 1.0f) * image.height;

        color = image.pixels[imagex + imagey * image.height];
        if (color.a < 255) {
            isFilled = false;
        }
        triangles.clear();
        triangles.shrink_to_fit();
        return;
    }

    if (isLeaf) return;

    constexpr vec3 offsets[] = {
        vec3(-1, -1, -1),
        vec3(1, -1, -1),
        vec3(-1, 1, -1),
        vec3(1, 1, -1),
        vec3(-1, -1, 1),
        vec3(1, -1, 1),
        vec3(-1, 1, 1),
        vec3(1, 1, 1)
    };
    const vec3 center = boundingBox.GetCenter();
    const float childSize = boundingBox.GetSize().x / 4.0f;
    const float octantOffset = childSize / 2.0f;
    children = new Node[8];

    for (int i = 0; i < 8; ++i) {
        children[i].octree = octree;
        children[i].parent = this;
        children[i].boundingBox = {center + octantOffset * offsets[i], childSize};
    }

    bool nodesToDivide[8] = {};

    for (int i = 0; i < 8; ++i) {
        for (const auto &triangle: triangles) {
            if (Intersect(children[i].boundingBox, triangle)) {
                children[i].triangles.push_back(triangle);
            }
        }

        if (!children[i].triangles.empty()) {
            nodesToDivide[i] = true;
        }
    }

    triangles.clear();
    triangles.shrink_to_fit();
    for (int i = 0; i < 8; ++i) {
        if (nodesToDivide[i]) {
            children[i].Subdivide(subdivisions - 1);
        } else {
            children[i].isLeaf = true;
        }
    }
}

void
OctreeMesh::Node::Linearize(const uint32 level, const ivec3 &globalPosition, std::vector<Color> &arr) {
    if (isFilled) {
        const uint32 gridSize = octree->GetSize();
        const uint32 voxelSize = gridSize >> level;
        for (uint32 z = 0; z < voxelSize; ++z) {
            for (uint32 y = 0; y < voxelSize; ++y) {
                for (uint32 x = 0; x < voxelSize; ++x) {
                    const uint32 index = Flatten(globalPosition + ivec3(x, y, z), ivec3(gridSize));
                    arr[index] = color;
                }
            }
        }
    }

    if (isLeaf) return;

    for (int i = 0; i < 8; i++) {
        const uint32 gridSize = octree->GetSize();
        const uint32 voxelSize = gridSize >> level;
        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
        children[i].Linearize(level + 1, ivec3(x, y, z), arr);
    }
}

void
OctreeMesh::Node::BrickMapify(const uint32 level, const ivec3 &globalPosition, BrickMap &bm) {
    if (isFilled) {
        const uint32 gridSize = octree->GetSize();
        const uint32 voxelSize = gridSize >> level;
        for (uint32 z = 0; z < voxelSize; ++z) {
            for (uint32 y = 0; y < voxelSize; ++y) {
                for (uint32 x = 0; x < voxelSize; ++x) {
                    bm.Insert(
                        globalPosition.x + x,
                        globalPosition.y + y,
                        globalPosition.z + z,
                        color);
                }
            }
        }
    }

    if (isLeaf) return;

    for (int i = 0; i < 8; i++) {
        const uint32 gridSize = octree->GetSize();
        const uint32 voxelSize = gridSize >> level;
        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
        children[i].BrickMapify(level + 1, ivec3(x, y, z), bm);
    }
}

void
OctreeMesh::Node::Clear() {
    delete[] children;
    children = nullptr;
}

