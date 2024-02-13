#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Model.h"

class MeshResource
{
public:
	static MeshResource* Instance();

	std::shared_ptr<Model> CreateCube(float width, float height, float depth);
	MeshResource CreateSphere(float radius, int resolution);
	std::shared_ptr<Model> LoadModel(std::string path);

	//void Bind();
	
private:
	static MeshResource* instance;

	std::shared_ptr<Model> LoadOBJ(std::string path);
	std::shared_ptr<Model> LoadGLTF(std::string path);
	void Upload(std::shared_ptr<Model> model);

    std::unordered_map<std::string, std::shared_ptr<Model>> loadedModels;
	MeshResource() {}
};