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


OctreeMesh::OctreeMesh(const Mesh &mesh, const uint32 depth)
    : m_MaxDepth(depth) {
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
    m_Nodes.emplace_back(rootBounds, 0, triangles, 0);
    m_Size = std::exp2(m_MaxDepth);

    std::cout << "Octree dimensions: " << m_Size << '\n';
    std::cout << "Subdividing mesh.\n";
    Subdivide(0, 0);
}

OctreeMesh::OctreeMesh(const Model &model, const uint32 depth)
    : m_MaxDepth(depth) {
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
    m_Nodes.emplace_back(rootBounds, 0, triangles, 0);
    m_Size = std::exp2(m_MaxDepth);

    std::cout << "Octree dimensions: " << m_Size << '\n';
    std::cout << "Subdividing mesh.\n";
    Subdivide(0, 0);
    std::cout << "Number of nodes: " << m_Nodes.size() << '\n';
}

void
OctreeMesh::Subdivide(const uint32 nodeIndex, const uint32 depth) {
    if (depth == m_MaxDepth) {
        Node &node = m_Nodes[nodeIndex];
        //node.color = {255, 255, 255, 255};
        //return;
        for (const auto &triangle: node.triangles) {
            float b0, b1, b2;
            if (triangle.PointInTriangle(node.boundingBox.GetCenter(), &b0, &b1, &b2)) {
                const Image &image = ImageManager::Get().GetImage(triangle.imageId);
                vec2 uv = {
                    b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x,
                    b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y
                };

                //uv = {
                //    std::fmod(uv.x, 1.0f),
                //    std::fmod(uv.y, 1.0f)
                //};

                const int imagex = static_cast<int>(uv.x * image.width) % image.width;
                const int imagey = static_cast<int>(uv.y * image.height) % image.height;
                //node.color = {
                //    static_cast<uint8>(uv.x * 255),
                //    static_cast<uint8>(uv.y * 255),
                //    static_cast<uint8>(triangle.imageId),
                //    255
                //};
                node.color = image.pixels[imagex + imagey * image.height];

                break;
            }
        }
        if (node.color.data == 0) {
            const Triangle &triangle = node.triangles[0];
            const Image &image = ImageManager::Get().GetImage(triangle.imageId);

            const int imagex = static_cast<int>(triangle.a.uv.x * image.width) % image.width;
            const int imagey = static_cast<int>(triangle.a.uv.y * image.height) % image.height;
            node.color = image.pixels[imagex + imagey * image.height];
        }

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

std::vector<Color>
OctreeMesh::Linearize() {
    std::vector<Color> voxels(m_Size * m_Size * m_Size);
    //m_Root.Linearize(0, ivec3(0), voxels);
    return voxels;
}

BrickMap
OctreeMesh::CreateBrickMap(const float voxelSize) {
    std::cout << "Creating brickmap.\n";
    BrickMap bm(ivec3(m_Size), voxelSize);
    FillBrickMap(&m_Nodes[0], 0, {0, 0, 0}, bm);
    return bm;
}


uint32
OctreeMesh::GetSize() const {
    return m_Size;
}

void
OctreeMesh::FillBrickMap(Node *node, const uint32 level, const ivec3 &globalPosition, BrickMap &bm) {
    if (node->color.a > 0) {
        const uint32 voxelSize = m_Size >> level;

        for (uint32 z = 0; z < voxelSize; ++z) {
            for (uint32 y = 0; y < voxelSize; ++y) {
                for (uint32 x = 0; x < voxelSize; ++x) {
                    bm.Insert(
                        globalPosition.x + x,
                        globalPosition.y + y,
                        globalPosition.z + z,
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


//void
//OctreeMesh::Node::Draw() {
//    if (isLeaf || parent == nullptr) {
//        Debug::DrawBox(boundingBox.GetCenter(), {}, vec3(boundingBox.GetSize() / 2.0f), vec4(0, 1, 0, 1), 2);
//        sentDrawCalls++;
//    }
//    if (isLeaf) return;
//
//    for (int i = 0; i < 8; ++i) {
//        children[i].Draw();
//    }
//}
//
//void
//OctreeMesh::Node::Subdivide(const uint32 subdivisions) {
//    //std::cout << triangles.size() << '\n';
//    if (subdivisions == 0) {
//        isLeaf = true;
//
//        for (const auto &triangle: triangles) {
//            if (Intersect(boundingBox, triangle)) {
//                isFilled = true;
//                break;
//            }
//        }
//
//        const vec3 center = boundingBox.GetCenter();
//        vec2 uv;
//        for (const auto &triangle: triangles) {
//            float b0, b1, b2;
//            if (triangle.PointInTriangle(center, &b0, &b1, &b2)) {
//                uv = {
//                    b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x,
//                    b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y
//                };
//                break;
//            }
//            assert(!std::isnan(uv.x) && !std::isnan(uv.y));
//        }
//        const Image &image = ImageManager::Get().GetImage(triangles[0].imageId);
//        const int imagex = std::fmod(uv.x, 1.0f) * image.width;
//        const int imagey = std::fmod(uv.y, 1.0f) * image.height;
//
//        color = image.pixels[imagex + imagey * image.height];
//        if (color.a < 255) {
//            isFilled = false;
//        }
//        triangles = {};
//        return;
//    }
//
//    if (isLeaf) return;
//
//    constexpr vec3 offsets[] = {
//        vec3(-1, -1, -1),
//        vec3(1, -1, -1),
//        vec3(-1, 1, -1),
//        vec3(1, 1, -1),
//        vec3(-1, -1, 1),
//        vec3(1, -1, 1),
//        vec3(-1, 1, 1),
//        vec3(1, 1, 1)
//    };
//    const vec3 center = boundingBox.GetCenter();
//    const float childSize = boundingBox.GetSize().x / 4.0f;
//    const float octantOffset = childSize / 2.0f;
//    children = new Node[8];
//
//    for (int i = 0; i < 8; ++i) {
//        children[i].octree = octree;
//        children[i].parent = this;
//        children[i].boundingBox = {center + octantOffset * offsets[i], childSize};
//    }
//
//    bool nodesToDivide[8] = {};
//
//    for (int i = 0; i < 8; ++i) {
//        for (const auto &triangle: triangles) {
//            if (Intersect(children[i].boundingBox, triangle)) {
//                children[i].triangles.push_back(triangle);
//            }
//        }
//
//        if (!children[i].triangles.empty()) {
//            nodesToDivide[i] = true;
//        }
//    }
//
//    triangles = {};
//    for (int i = 0; i < 8; ++i) {
//        if (nodesToDivide[i]) {
//            children[i].Subdivide(subdivisions - 1);
//        } else {
//            children[i].isLeaf = true;
//        }
//    }
//}
//
//void
//OctreeMesh::Node::Linearize(const uint32 level, const ivec3 &globalPosition, std::vector<Color> &arr) {
//    if (isFilled) {
//        const uint32 gridSize = octree->GetSize();
//        const uint32 voxelSize = gridSize >> level;
//        for (uint32 z = 0; z < voxelSize; ++z) {
//            for (uint32 y = 0; y < voxelSize; ++y) {
//                for (uint32 x = 0; x < voxelSize; ++x) {
//                    const uint32 index = Flatten(globalPosition + ivec3(x, y, z), ivec3(gridSize));
//                    arr[index] = color;
//                }
//            }
//        }
//    }
//
//    if (isLeaf) return;
//
//    for (int i = 0; i < 8; i++) {
//        const uint32 gridSize = octree->GetSize();
//        const uint32 voxelSize = gridSize >> level;
//        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
//        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
//        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
//        children[i].Linearize(level + 1, ivec3(x, y, z), arr);
//    }
//}
//
//void
//OctreeMesh::Node::BrickMapify(const uint32 level, const ivec3 &globalPosition, BrickMap &bm) {
//    if (isFilled) {
//        const uint32 gridSize = octree->GetSize();
//        const uint32 voxelSize = gridSize >> level;
//        for (uint32 z = 0; z < voxelSize; ++z) {
//            for (uint32 y = 0; y < voxelSize; ++y) {
//                for (uint32 x = 0; x < voxelSize; ++x) {
//                    bm.Insert(
//                        globalPosition.x + x,
//                        globalPosition.y + y,
//                        globalPosition.z + z,
//                        color);
//                }
//            }
//        }
//    }
//
//    if (isLeaf) return;
//
//    for (int i = 0; i < 8; i++) {
//        const uint32 gridSize = octree->GetSize();
//        const uint32 voxelSize = gridSize >> level;
//        const int x = globalPosition.x + (i & 1 ? voxelSize / 2 : 0);
//        const int y = globalPosition.y + (i & 2 ? voxelSize / 2 : 0);
//        const int z = globalPosition.z + (i & 4 ? voxelSize / 2 : 0);
//        children[i].BrickMapify(level + 1, ivec3(x, y, z), bm);
//    }
//}
//
//void
//OctreeMesh::Node::Clear() {
//    delete[] children;
//    children = nullptr;
//}

ModelBVH::Node::Node(const uint32 triangleIndex)
    : boundingBox(vec3(FLT_MAX), vec3(-FLT_MAX)),
      triangleIndex(triangleIndex) {
}

void
ModelBVH::Split(Node *node, const uint32 depth) {
    if (depth == m_MaxDepth || node->triangleCount < 10) return;

    const vec3 size = node->boundingBox.GetSize();
    const uint32 splitAxis = size.x > std::max(size.y, size.z) ? 0 : size.y > std::max(size.x, size.z) ? 1 : 2;
    const float splitPosition = node->boundingBox.GetCenter()[splitAxis];
    node->childIndex = m_Nodes.size();
    m_Nodes.emplace_back(node->triangleIndex);
    m_Nodes.emplace_back(node->triangleIndex);
    Node *childA = &m_Nodes[node->childIndex];
    Node *childB = &m_Nodes[node->childIndex + 1];
    for (uint32 i = node->triangleIndex; i < node->triangleIndex + node->triangleCount; ++i) {
        Triangle &triangle = m_Triangles[i];

        const bool isA = triangle.GetCenter()[splitAxis] < splitPosition;
        Node *child = isA ? childA : childB;
        //child->boundingBox.GrowToInclude(triangle.a.position);
        //child->boundingBox.GrowToInclude(triangle.b.position);
        //child->boundingBox.GrowToInclude(triangle.c.position);

        child->boundingBox.min = min(child->boundingBox.min,
                                     min(triangle.a.position, min(triangle.b.position, triangle.c.position)));
        child->boundingBox.max = max(child->boundingBox.max,
                                     max(triangle.a.position, max(triangle.b.position, triangle.c.position)));

        child->triangleCount++;

        if (isA) {
            const uint32 swapIndex = child->triangleIndex + child->triangleCount - 1;
            std::swap(m_Triangles[i], m_Triangles[swapIndex]);
            childB->triangleIndex++;
        }
    }

    Split(&m_Nodes[node->childIndex], depth + 1);
    Split(&m_Nodes[node->childIndex + 1], depth + 1);
}


//ModelBVH::Node *
//ModelBVH::Node::GetChildren() const {
//    if (childIndex == 0) {
//        return nullptr;
//    }
//    return &container->m_Nodes[childIndex];
//}
//
//std::pair<Triangle *, uint32>
//ModelBVH::Node::GetTriangles() const {
//    return {&container->m_Triangles[triangleIndex], triangleCount};
//}

bool
ModelBVH::Node::IsLeaf() const {
    return childIndex == 0;
}

ModelBVH::ModelBVH(const Model &model, const uint32 maxDepth)
    : m_MaxDepth(maxDepth) {
    size_t numTriangles = 0;
    for (const auto &mesh: model.meshes) {
        // Assumes mesh has triangular faces.
        numTriangles += mesh.indices.size() / 3;
    }

    m_Triangles.reserve(numTriangles);

    BoundingBox rootBounds = {vec3(FLT_MAX), vec3(-FLT_MAX)};

    for (auto &mesh: model.meshes) {
        const uint32 imageId = mesh.image.id;

        for (unsigned i = 0; i < mesh.indices.size(); i += 3) {
            const Triangle &triangle = m_Triangles.emplace_back(
                mesh.vertices[mesh.indices[i]],
                mesh.vertices[mesh.indices[i + 1]],
                mesh.vertices[mesh.indices[i + 2]],
                imageId
            );

            for (unsigned j = 0; j < 3; ++j) {
                rootBounds.GrowToInclude(triangle[j].position);
            }
        }
    }

    m_Nodes.emplace_back(0);
    Node *root = &m_Nodes[0];

    root->triangleCount = m_Triangles.size();
    root->boundingBox = rootBounds;
    Split(root, 0);

    std::cout << "Created " << m_Nodes.size() << " BVH nodes.\n";
}

void
ModelBVH::Draw(const int32 level, const vec3 scale) {
    m_DrawScale = scale;
    DrawImpl(&m_Nodes[0], level, {255, 0, 0, 255});
}

void
ModelBVH::DrawImpl(const Node *node, const uint32 level, const Color color) const {
    if (node->childIndex == 0 || level == 0) {
        const vec4 v4Color = {
            static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f,
            static_cast<float>(color.a) / 255.0f
        };
        const int lineWitdth = color.r > 0 ? 3 : 2;
        Debug::DrawBox(node->boundingBox.GetCenter() * m_DrawScale, vec3(0.0f),
                       node->boundingBox.GetSize() * m_DrawScale, v4Color, lineWitdth);
        return;
    }

    const uint32 childIndex = node->childIndex;
    DrawImpl(&m_Nodes[childIndex + 0], level - 1, {255, 0, 0, 255});
    DrawImpl(&m_Nodes[childIndex + 1], level - 1, {0, 255, 0, 255});
}
