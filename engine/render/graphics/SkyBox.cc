#include "config.h"
#include "SkyBox.h"

void
SkyBox::Init()
{
	// Define vertices & indices
	float vertices[] = {
    -1.0f, -1.0f, -1.0f, 
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 
    1.0f, 1.0f, -1.0f, 
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, 
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
	};

	// Vertices
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	std::vector<std::string> paths = {
		"Assets/Sky/SkyR.png",
		"Assets/Sky/SkyL.png",
		"Assets/Sky/SkyU.png",
		"Assets/Sky/SkyD.png",
		"Assets/Sky/SkyF.png",
		"Assets/Sky/SkyB.png",
	};

	texture = TextureResource::Instance()->LoadCubeMap(paths);
}

void 
SkyBox::Draw(Camera cam, GLuint program)
{
	glUseProgram(program);
    UniformMat4("invProj", inverse(cam.projection), program);
    UniformMat4("invView", inverse(cam.view), program);

	glBindVertexArray(this->vao);

	texture.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);
}
