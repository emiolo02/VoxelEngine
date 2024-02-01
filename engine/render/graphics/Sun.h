#pragma once
#include "core/app.h"

class Sun
{
public:
	vec3 color;
	vec3 direction;
	float intensity = 0;

	Sun();
	Sun(vec3 color, vec3 direction, float intensity);
	void SetProperties(vec3 color, vec3 direction, float intensity);
	void Update(GLuint program);
	void Disable(GLuint program);
};