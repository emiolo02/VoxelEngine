#include "Voxelizer.hpp"
#include "Model.hpp"
#include "DataStructures/BrickMap.hpp"
#include "Render/Debug.hpp"
#include <thread>

bool
Intersect(const math::BoundingBox &box, const Triangle &tri) {
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


OctreeMesh::OctreeMesh(const Mesh &mesh, const uint32 depth)
    : m_MaxDepth(depth) {
    std::vector<Triangle> triangles(mesh.indices.size() / 3);

    math::BoundingBox rootBounds = {vec3(FLT_MAX), vec3(-FLT_MAX)};

    const Material *material = &mesh.material;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        const Triangle &triangle = triangles[i / 3] = Triangle(
                                       mesh.vertices[mesh.indices[i]],
                                       mesh.vertices[mesh.indices[i + 1]],
                                       mesh.vertices[mesh.indices[i + 2]],
                                       material
                                   );

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
    m_Nodes.emplace_back(rootBounds, 0, triangles, math::Color());
    m_Size = std::exp2(m_MaxDepth);

    std::cout << "Octree dimensions: " << m_Size << '\n';
    std::cout << "Subdividing mesh.\n";
    Subdivide(0, 0);
}

//std::string
//PrefixedSize(const float size) {
//    if (size > 1e9f)
//        return std::to_string(size / 1e9f) + " gb";
//    if (size > 1e6f)
//        return std::to_string(size / 1e6f) + " mb";
//    if (size > 1e3f)
//        return std::to_string(size / 1e3f) + " kb";
//
//    return std::to_string(size) + " b";
//}

OctreeMesh::OctreeMesh(const Model &model, const uint32 depth)
    : m_MaxDepth(depth) {
    std::vector<Triangle> triangles;
    math::BoundingBox rootBounds = {vec3(FLT_MAX), vec3(-FLT_MAX)};

    for (auto &mesh: model.meshes) {
        const Material *material = &mesh.material;

        for (unsigned i = 0; i < mesh.indices.size(); i += 3) {
            const Triangle &triangle = triangles.emplace_back(
                mesh.vertices[mesh.indices[i]],
                mesh.vertices[mesh.indices[i + 1]],
                mesh.vertices[mesh.indices[i + 2]],
                material
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
    m_Nodes.emplace_back(rootBounds, 0, triangles, math::Color());
    m_Size = std::exp2(m_MaxDepth);

    std::cout << "Octree dimensions: " << m_Size << '\n';
    std::cout << "Subdividing mesh.\n";
    Subdivide(0, 0);
    std::cout << "Number of nodes: " << m_Nodes.size() << '\n';
    std::cout << "\tSize: " << PrefixedSize(m_Nodes.size() * sizeof(Node)) << '\n';
}

void
OctreeMesh::Subdivide(const uint32 nodeIndex, const uint32 depth) {
    if (depth == m_MaxDepth) {
        Node &node = m_Nodes[nodeIndex];
        for (const auto &triangle: node.triangles) {
            float b0, b1, b2;
            if (triangle.PointInTriangle(node.boundingBox.GetCenter(), &b0, &b1, &b2)) {
                if (triangle.material->texture) {
                    const std::shared_ptr<Image> image = TextureManager::Get().GetTextureImage(
                        triangle.material->texture->GetId());

                    const vec2 uv = {
                        b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x,
                        b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y
                    };

                    const int imagex = static_cast<int>(fract(uv.x) * image->width);
                    const int imagey = static_cast<int>(fract(uv.y) * image->height);
                    node.color = image->pixels[imagex + imagey * image->height];
                } else {
                    node.color = math::Color(triangle.material->baseColor);
                }
                break;
            }
        }
        //if (node.color.data == 0) node.color = math::Color(0xFFFFFFFF);

        node.triangles.clear();
        node.triangles.shrink_to_fit();
        return;
    }

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

    m_Nodes.insert(m_Nodes.end(), 8, {});

    //m_Nodes[nodeIndex]

    const vec3 center = m_Nodes[nodeIndex].boundingBox.GetCenter();
    const float childSize = m_Nodes[nodeIndex].boundingBox.GetSize().x / 2.0f;
    const float octantOffset = childSize / 2.0f;

    m_Nodes[nodeIndex].childIndex = m_Nodes.size() - 8;

    for (int i = 0; i < 8; ++i) {
        Node &child = m_Nodes[m_Nodes[nodeIndex].childIndex + i];
        //.emplace_back(BoundingBox(center + octantOffset * offsets[i], childSize), 0, -1, Color());
        child.boundingBox = {center + octantOffset * offsets[i], childSize};

        for (const auto &triangle: m_Nodes[nodeIndex].triangles) {
            if (Intersect(child.boundingBox, triangle)) {
                child.triangles.push_back(triangle);
            }
        }
    }

    m_Nodes[nodeIndex].triangles.clear();
    m_Nodes[nodeIndex].triangles.shrink_to_fit();

    for (int i = 0; i < 8; ++i) {
        Node &child = m_Nodes[m_Nodes[nodeIndex].childIndex + i];
        if (!child.triangles.empty()) {
            Subdivide(m_Nodes[nodeIndex].childIndex + i, depth + 1);
        }
    }
}


void
OctreeMesh::Draw() const {
    if (!m_Nodes.empty()) {
        DrawNode(&m_Nodes[0], 0);
    }
}

void
OctreeMesh::DrawNode(const Node *node, const uint32 depth) const {
    constexpr vec4 depthColor[] = {
        {0, 0, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {1, 0, 0, 1},
        {1, 0, 1, 1},
        {1, 1, 0, 1},
        {1, 1, 1, 1}
    };
    if (depth == m_MaxDepth) {
        Debug::DrawBox(node->boundingBox.GetCenter(), {}, node->boundingBox.GetSize(), depthColor[depth % 7], 2);
    }
    if (node->childIndex == 0) {
        // Is leaf node.

        return;
    }


    for (uint32 i = 0; i < 8; ++i) {
        const Node *child = &m_Nodes[node->childIndex + i];
        DrawNode(child, depth + 1);
    }
}

BrickMap
OctreeMesh::CreateBrickMap(const float voxelSize) {
    std::cout << "Creating brickmap.\n";
    BrickMap bm(ivec3(m_Size), voxelSize);
    FillBrickMap(&m_Nodes[0], 0, {0, 0, 0}, bm);
    return bm;
}

BrickMap
OctreeMesh::CreateBrickMap(const float voxelSize, const math::Color color) {
    std::cout << "Creating brickmap.\n";

    BrickMap bm(ivec3(m_Size), voxelSize);

    const uint32 textureIndex = bm.GetBrickTextures().size();
    auto &texture = bm.GetBrickTextures().emplace_back();
    for (uint32 i = 0; i < BRICK_SIZE; ++i) {
        texture.voxels[i] = color;
    }

    FillBrickMap(&m_Nodes[0], 0, {0, 0, 0}, bm, textureIndex);
    return bm;
}


uint32
OctreeMesh::GetSize() const {
    return m_Size;
}

void
OctreeMesh::FillBrickMap(const Node *node,
                         const uint32 level,
                         const ivec3 &globalPosition,
                         BrickMap &bm) {
    if (node->color.data) {
        const uint32 voxelSize = m_Size >> level;

        for (uint32 z = 0; z < voxelSize; ++z) {
            for (uint32 y = 0; y < voxelSize; ++y) {
                for (uint32 x = 0; x < voxelSize; ++x) {
                    bm.Insert(
                        globalPosition + ivec3(x, y, z),
                        node->color
                    );
                }
            }
        }
        return;
    }

    if (node->childIndex == 0) {
        return;
    }

    for (uint32 i = 0; i < 8; ++i) {
        const uint32 voxelSize = m_Size >> level;
        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
        FillBrickMap(&m_Nodes[node->childIndex + i], level + 1, {x, y, z}, bm);
    }
}

void OctreeMesh::FillBrickMap(const Node *node,
                              const uint32 level,
                              const ivec3 &globalPosition,
                              BrickMap &bm,
                              const uint32 textureIndex) {
    if (node->color.data) {
        const uint32 voxelSize = m_Size >> level;

        for (uint32 z = 0; z < voxelSize; ++z) {
            for (uint32 y = 0; y < voxelSize; ++y) {
                for (uint32 x = 0; x < voxelSize; ++x) {
                    bm.Insert(
                        globalPosition + ivec3(x, y, z),
                        textureIndex
                    );
                }
            }
        }
        return;
    }

    if (node->childIndex == 0) {
        return;
    }

    for (uint32 i = 0; i < 8; ++i) {
        const uint32 voxelSize = m_Size >> level;
        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
        FillBrickMap(&m_Nodes[node->childIndex + i], level + 1, {x, y, z}, bm, textureIndex);
    }
}

BrickMap
Voxelize(const Model &model, const uint32 subdivisions, const float voxelSize) {
    OctreeMesh octree(model, subdivisions);
    return octree.CreateBrickMap(voxelSize);
}

BrickMap
Voxelize(const Model &model, const uint32 subdivisions, const float voxelSize, const math::Color color) {
    OctreeMesh octree(model, subdivisions);
    return octree.CreateBrickMap(voxelSize, color);
}
