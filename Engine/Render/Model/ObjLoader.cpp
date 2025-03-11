#include "ObjLoader.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Render/Texture/Image.hpp"


Mesh
ObjLoader::ProcessMesh(const aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<uint32> indices;
    indices.reserve(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const vec2 uv = mesh->mTextureCoords[0]
                            ? *reinterpret_cast<vec2 *>(&mesh->mTextureCoords[0][i])
                            : vec2(0.0f);
        const vec3 normal = mesh->mNormals != nullptr ? *reinterpret_cast<vec3 *>(&mesh->mNormals[i]) : vec3(0.0f);
        vertices[i] = {
            *reinterpret_cast<vec3 *>(&mesh->mVertices[i]),
            normal,
            uv
        };
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex &a = vertices[indices[i]];
        Vertex &b = vertices[indices[i + 1]];
        Vertex &c = vertices[indices[i + 2]];
        const vec3 edge0 = b.position - a.position;
        const vec3 edge1 = c.position - a.position;
        a.normal = normalize(cross(edge0, edge1));
        b.normal = normalize(cross(edge0, edge1));
        c.normal = normalize(cross(edge0, edge1));
        //if (a.normal == vec3(0)) {

        //}
        //if (b.normal == vec3(0)) {
        //    const vec3 edge0 = a.position - b.position;
        //    const vec3 edge1 = c.position - b.position;
        //    b.normal = normalize(cross(edge0, edge1));
        //}
        //if (c.normal == vec3(0)) {
        //    const vec3 edge0 = b.position - c.position;
        //    const vec3 edge1 = a.position - c.position;
        //    c.normal = normalize(cross(edge0, edge1));
        //}
    }

    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    aiString path;
    mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
    const std::string fullPath = m_Directory + path.C_Str();
    int32 imageId = -1;
    if (path.length > 0) {
        imageId = ImageManager::Get().Load(fullPath).id;
    }

    return {
        vertices,
        indices,
        imageId
    };
}

void
ObjLoader::ProcessNode(Model &model, const aiNode *node, const aiScene *scene) {
    model.meshes.reserve(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(model, node->mChildren[i], scene);
    }
}

const Model &
ObjLoader::Load(const std::string &path, const bool reload) {
    if (m_LoadedModels.contains(path) && !reload) {
        return m_LoadedModels[path];
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    m_LoadedModels[path] = {};

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model \"" << path << "\"\n" << importer.GetErrorString() << std::endl;
    } else {
        m_Directory = path.substr(0, path.find_last_of('/')) + "/";
        ProcessNode(m_LoadedModels[path], scene->mRootNode, scene);
        std::cout << "Loaded model \"" << path << "\"\n";
    }

    return m_LoadedModels[path];
}

void
ObjLoader::Remove(const std::string &name) {
    if (m_LoadedModels.contains(name)) {
        m_LoadedModels.erase(name);
    }
}
