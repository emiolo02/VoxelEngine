#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderResource.h"
#include "TextureResource.h"
#include "Camera.h"
#include <string>

class SkyBox
{
public:
	void Init();

	void Draw(Camera cam, GLuint program);

private:
	GLuint vbo = 0;
	GLuint vao = 0;

	Texture texture;
};