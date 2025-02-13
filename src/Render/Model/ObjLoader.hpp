#pragma once

#include "Model.hpp"

struct aiNode;
struct aiScene;
struct aiMesh;

class ObjLoader {
    SINGLETON(ObjLoader);

public:
    const Model &Load(const std::string &path, bool reload = false);

    void Remove(const std::string &name);

private:
    void ProcessNode(Model &model, const aiNode *node, const aiScene *scene);

    Mesh ProcessMesh(const aiMesh *mesh, const aiScene *scene);

    std::string m_Directory;
    std::unordered_map<std::string, Model> m_LoadedModels;
};
