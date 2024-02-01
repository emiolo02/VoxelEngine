#include "config.h"
#include "Sun.h"
#include "ShaderResource.h"

Sun::Sun()
{
	// Empty
}

Sun::Sun(vec3 color, vec3 direction, float intensity)
{
	this->color = color;
	this->direction = normalize(direction);
	this->intensity = intensity;
}
void
Sun::SetProperties(vec3 color, vec3 direction, float intensity)
{
	this->color = color;
	this->direction = normalize(direction);
	this->intensity = intensity;
}

void
Sun::Update(GLuint program)
{
	glUseProgram(program);

	UniformVec3("sun.color", this->color, program);
	UniformVec3("sun.direction", this->direction, program);
	UniformFloat("sun.intensity", this->intensity, program);
}

void
Sun::Disable(GLuint program)
{
	glUseProgram(program);

	UniformVec3("sun.color", vec3(), program);
	UniformVec3("sun.direction", vec3(), program);
	UniformFloat("sun.intensity", 0, program);
}