#pragma once
#include <string>
#include <memory>
#include "GL/glew.h"

class ShaderResource;

class PointLight
{
public:
	vec3 pos;
	vec3 color;
	float intensity = 0;
	float radius = 0;

	float constant = 1.0;
	float linear = 0.7;
	float quadratic = 1.8;

	PointLight();
	PointLight(vec3 pos, vec3 color, float intensity);
	void Update(GLuint program);
	void Disable(GLuint program);

	static int GetNumLights();
private:
	static int numLights;
	int index = 0;

	std::string posName;
	std::string colName;
	std::string intName;
	std::string radName;

	std::string constName;
	std::string linName;
	std::string quadName;
};