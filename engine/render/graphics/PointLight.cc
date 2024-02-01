#include "config.h"
#include "PointLight.h"
#include "ShaderResource.h"

int PointLight::numLights;

PointLight::PointLight()
{
	// Empty
}

PointLight::PointLight(vec3 pos, vec3 color, float intensity)
{
	this->pos = pos;
	this->color = color;
	this->intensity = intensity;

	float lightMax = std::max(std::max(color.x, color.y), color.z);
	this->radius = 
		(-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
		/ (2 * quadratic);
	this->radius *= intensity;

	this->index = this->numLights;
	this->numLights++;

	posName = "pointLights[" + std::to_string(this->index) + "].position";
	colName = "pointLights[" + std::to_string(this->index) + "].color";
	intName = "pointLights[" + std::to_string(this->index) + "].intensity";
	radName = "pointLights[" + std::to_string(this->index) + "].radius";

	constName = "pointLights[" + std::to_string(this->index) + "].constant";
	linName = "pointLights[" + std::to_string(this->index) + "].linear";
	quadName = "pointLights[" + std::to_string(this->index) + "].quadratic";
}

void PointLight::Update(GLuint program)
{
	float lightMax = std::max(std::max(color.x, color.y), color.z);
	this->radius =
		(-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
		/ (2 * quadratic);
	this->radius *= intensity;
	UniformVec3(posName.c_str(), this->pos, program);
	UniformVec3(colName.c_str(), this->color, program);
	UniformFloat(intName.c_str(), this->intensity, program);
	UniformFloat(radName.c_str(), this->radius, program);

	UniformFloat(constName.c_str(), this->constant, program);
	UniformFloat(linName.c_str(), this->linear, program);
	UniformFloat(quadName.c_str(), this->quadratic, program);
}

void
PointLight::Disable(GLuint program)
{
	glUseProgram(program);

	std::string colName = "pointLights[" + std::to_string(this->index) + "].color";
	std::string intName = "pointLights[" + std::to_string(this->index) + "].intensity";

	UniformVec3(colName.c_str(), vec3(), program);
	UniformFloat(intName.c_str(), 0, program);
}

int 
PointLight::GetNumLights()
{
	return numLights;
}