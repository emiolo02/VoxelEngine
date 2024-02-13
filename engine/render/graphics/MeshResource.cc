#include "config.h"
#include "MeshResource.h"
#include "GLTFinfo.h"
#include "TextureResource.h"
#include "Material.h"

MeshResource* MeshResource::instance = nullptr;

MeshResource*
MeshResource::Instance()
{
	if (instance == nullptr)
		instance = new MeshResource();

	return instance;
}


std::shared_ptr<Model>
MeshResource::CreateCube(float width, float height, float depth)
{
	Primitive prim;

	//front
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height,  0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width, -0.5f * height,  0.5f * depth), vec2(1.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width,  0.5f * height,  0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height,  0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	
	//bottom
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height,  0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width, -0.5f * height,  0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height, -0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width, -0.5f * height, -0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	
	//top
	prim.vertices.push_back(Vertex(vec3( 0.5f * width,  0.5f * height,  0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height,  0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(0.5f * width,   0.5f * height, -0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height, -0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	
	//left
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height,  0.5f * depth), vec2(1.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height,  0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height, -0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height, -0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	
	//right
	prim.vertices.push_back(Vertex(vec3(0.5f * width,  -0.5f * height,  0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(0.5f * width,   0.5f * height,  0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(0.5f * width,  -0.5f * height, -0.5f * depth), vec2(1.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(0.5f * width,   0.5f * height, -0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	
	//back
	prim.vertices.push_back(Vertex(vec3(-0.5f * width, -0.5f * height, -0.5f * depth), vec2(0.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width, -0.5f * height, -0.5f * depth), vec2(1.0f, 1.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3( 0.5f * width,  0.5f * height, -0.5f * depth), vec2(1.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));
	prim.vertices.push_back(Vertex(vec3(-0.5f * width,  0.5f * height, -0.5f * depth), vec2(0.0f, 0.0f), vec3(0, 0, 0), vec4(0, 0, 0, 0)));

	prim.indices =
	{
		0, 1, 2,
		2, 3, 0,
	
		4, 5, 6,
		6, 7, 5,
	
		8, 9, 10,
		10, 11, 9,
	
		12, 13, 14,
		14, 15, 13,
	
		16, 17, 18,
		18, 19, 17,
	
		20, 21, 22,
		22, 23, 20
	};
	Mesh mesh(prim);
	
	return std::make_shared<Model>(mesh);
}

MeshResource
MeshResource::CreateSphere(float radius, int resolution)
{
	MeshResource mesh;
	//
	//float PI = 3.14159;
	//
	//float phi, theta;
	//float x, y, z;
	//
	//float latStep = PI / resolution;
	//float longStep = 2 * PI / resolution;
	//
	//
	//for (int lat = 0; lat < resolution; ++lat)
	//{
	//	phi = PI / 2 - lat * latStep;
	//
	//	y = radius * sin(phi);
	//
	//	for (int longi = 0; longi < resolution; ++longi)
	//	{
	//		theta = longi * longStep;
	//
	//		x = radius * cos(phi) * cos(theta);
	//		z = radius * cos(phi) * sin(theta);
	//
	//		mesh.pos.push_back(x);
	//		mesh.pos.push_back(y);
	//		mesh.pos.push_back(z);
	//
	//		mesh.texCoord.push_back((float)longi / resolution);
	//		mesh.texCoord.push_back((float)lat / resolution);
	//	}
	//}
	//
	//int p1, p2;
	//
	//for (int lat = 0; lat < resolution; ++lat)
	//{
	//	p1 = lat * (resolution + 1);
	//	p2 = p1 + resolution + 1;
	//
	//	for (int longi = 0; longi < resolution; ++longi, ++p1, ++p2)
	//	{
	//		if (lat != 0)
	//		{
	//			mesh.ind.push_back(p1);
	//			mesh.ind.push_back(p2);
	//			mesh.ind.push_back(p1 + 1);
	//		}
	//		if (lat != resolution-1)
	//		{
	//			mesh.ind.push_back(p1+1);
	//			mesh.ind.push_back(p2);
	//			mesh.ind.push_back(p2 + 1);
	//		}
	//
	//		mesh.ind.push_back(p1);
	//		mesh.ind.push_back(p2);
	//		if (lat != 0)
	//		{
	//			mesh.ind.push_back(p1);
	//			mesh.ind.push_back(p1 + 1);
	//		}
	//	}
	//}
	//
	return mesh;
}

std::shared_ptr<Model>
MeshResource::LoadModel(std::string path)
{
	if (loadedModels.find(path) != loadedModels.end())
		return loadedModels[path]; // Ignore if mesh already loaded

	std::ifstream file(path.c_str());
	if (!file.good())
	{
		std::cout << path << " - [ENGINE ERROR] file does not exist" << std::endl;
		return LoadModel("Assets/error.gltf");
	}

	std::shared_ptr<Model> model;
	
	std::vector<std::string> out;

	split(path, '.', out);
	std::string fileExt = out[out.size() - 1];

	if (fileExt == "obj")
		model = LoadOBJ(path);
	else if (fileExt == "gltf" || fileExt == "glb")
		model = LoadGLTF(path);
	else
	{
		std::cout << path << " - [ENGINE ERROR] loading unsupported file format" << std::endl;
		return LoadModel("Assets/error.gltf");
	}

	Upload(model);
	loadedModels[path] = model;

	return model;
}

std::string 
GetTexPath(std::string mtlPath)
{
	std::ifstream stream(mtlPath);
	std::string line;

	if (stream.good() == false)
	{
		std::cout << "Could not find file at " << mtlPath << std::endl;
		return "";
	}

	while (getline(stream, line))
	{
		std::vector<std::string> lineContent;
		split(line, ' ', lineContent);

		if (lineContent.size() == 0)
			continue;

		if (lineContent[0] != "#")
		{
			if (lineContent[0] == "map_Kd")
				return lineContent[1];
		}
	}

	std::cout << "Could not find texture path at " << mtlPath << std::endl;
}

std::shared_ptr<Model>
MeshResource::LoadOBJ(std::string path)
{
	// @TODO make loader support multiple primitives
	std::ifstream stream(path);

	std::string line;

	if (stream.good() == false)
	{
		std::cout << "Could not find file at " << path << std::endl;
		return loadedModels["Assets/error.glb"];
	}

	std::vector<vec3> positions;
	std::vector<vec2> UVs;
	std::vector<vec3> normals;
	std::vector<GLuint> indPos;
	std::vector<GLuint> indUV;
	std::vector<GLuint> indNor;
	std::string texturePath;

	// Read obj data
	while (getline(stream, line))
	{
		std::vector<std::string> lineContent;
		split(line, ' ', lineContent);

		std::string prefix = lineContent[0];

		if (prefix[0] == '#')
			continue;

		if (prefix == "mtllib")
		{
			std::string mtlFileName = lineContent.back();


			std::string mtlPath;
			std::vector<std::string> out;
			split(path, '/', out);

			for (int i = 0; i < out.size() - 1; i++)
				mtlPath.append(out[i] + "/");

			//texturePath = mtlPath + GetTexPath(mtlPath + mtlFileName);
		}

		if (prefix == "v")
		{
			std::vector<std::string> out;

			split(line, ' ', out);
			vec3 position;

			for (int i = 1; i < 4; i++)
				position[i - 1] = std::stof(out[i]);

			positions.push_back(position);
		}

		if (prefix == "vt")
		{
			std::vector<std::string> out;
			
			split(line, ' ', out);
			vec2 UV = vec2(std::stof(out[1]), std::stof(out[2]));

			UVs.push_back(UV);
		}

		if (prefix == "vn")
		{
			std::vector<std::string> out;

			split(line, ' ', out);
			vec3 normal;

			for (int i = 1; i < 4; i++)
				normal[i - 1] = std::stof(out[i]);

			normals.push_back(normal);
		}

		if (prefix == "f")
		{
			std::vector<std::string> out;

			split(line, ' ', out);
			
			for (int i = 1; i < 4; i++)
			{
				std::vector<std::string> out1; //splits indices
				split(out[i], '/', out1);
				indPos.push_back((GLuint) std::stoi(out1[0]) - 1);
				if (out1.size() > 1)
				indUV.push_back( (GLuint) std::stoi(out1[1]) - 1);
				if (out1.size() > 2)
				indNor.push_back((GLuint) std::stoi(out1[2]) - 1);
			}
		}
	}

	// Insert data from file
	std::vector<Vertex> vertices;
	vertices.resize(indPos.size());

	for (int i = 0; i < indPos.size(); ++i)
	{
		vertices[i].position = positions[indPos[i]];
		if (indUV.size() > 0)
			vertices[i].UV = UVs[indUV[i]];
		else
			vertices[i].UV = vec2(0, 0);
		if (indNor.size() > 0)
			vertices[i].normal = normals[indNor[i]];
		else
			vertices[i].normal = vec3(0, 0, 0);

		vertices[i].tangent = vec4(0, 0, 0, 0);
	}


	Primitive prim;
	for (int i = 0; i < indPos.size(); i++)
	{
		bool found = false;
		for (int j = 0; j < prim.vertices.size(); j++)
		{
			if (vertices[i] == prim.vertices[j])
			{
				prim.indices.push_back(j);
				found = true;
			}
		}
		if (!found)
		{
			prim.vertices.push_back(vertices[i]);
			prim.indices.push_back(prim.vertices.size() - 1);
		}
	}

	// Load textures
	auto texRes = TextureResource::Instance();
	TextureData albedo = texRes->LoadTexture(texturePath, true, GL_NEAREST);
	albedo.type = COLOR;
	TextureData normal = texRes->GetDefaultNormal();
	normal.type = NORMAL;
	TextureData emmisive = texRes->GetDefaultBlack();
	emmisive.type = EMMISIVE;
	TextureData occlusion = texRes->GetDefaultWhite();
	occlusion.type = OCCLUSION;
	TextureData pbr = texRes->GetDefaultBlack();
	pbr.type = METALLIC_ROUGHNESS;

	prim.material = new BlinnPhongMaterial();

	prim.material->AddTexture(Texture(albedo));
	prim.material->AddTexture(Texture(normal));
	prim.material->AddTexture(Texture(emmisive));
	prim.material->AddTexture(Texture(occlusion));
	prim.material->AddTexture(Texture(pbr));
	Mesh mesh(prim);

	return std::make_shared<Model>(mesh);
}

std::shared_ptr<Model>
MeshResource::LoadGLTF(std::string path)
{
	return GLTFinfo::LoadGLTF(path);
}

void 
MeshResource::Upload(std::shared_ptr<Model> model)
{
	for (auto& mesh : model->meshes)
	{
		for (auto& primitive : mesh.primitives)
		{
			/*////   Setup vbo     ////*/
			glGenVertexArrays(1, &primitive.vao);
			glBindVertexArray(primitive.vao);
			glGenBuffers(1, &primitive.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, primitive.vbo);
			
			glBufferData(
				GL_ARRAY_BUFFER, 
				sizeof(Vertex) * primitive.vertices.size(), 
				primitive.vertices.data(), GL_STATIC_DRAW);

			// Upload vertex data
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, UV));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));


			/*////   Setup ibo     ////*/

			glGenBuffers(1, &primitive.ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.ibo);
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, 
				primitive.indices.size() * sizeof(GLuint), 
				primitive.indices.data(), GL_STATIC_DRAW);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}