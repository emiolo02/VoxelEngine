#pragma once
#include "core/app.h"
#include "ShaderResource.h"
#include <vector>
#include <iostream>
enum GBUFFER_TYPE 
{
	GBUFFER_TYPE_ALBEDO,
	GBUFFER_TYPE_POSITION,
	GBUFFER_TYPE_NORMAL,
	GBUFFER_TYPE_SHADOW,
	GBUFFER_NUM_TEXTURES
};


class Quad
{
public:
	Quad(){}
	
	void Init();

	void Bind();
private:
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint quadIBO = 0;
};

// @TODO - split this into 2 subclasses: gbuffer & lightcull buffer
class FrameBuffer
{
public:
	FrameBuffer()
	{

	}

	~FrameBuffer()
	{
		glDeleteBuffers(1, &fbo);
	}

	void Init(int windowWidth, int windowHeight, int buffers = 1);
	void InitShadowBuffer(int width, int height);
	void InitGBuffer(int windowWidth, int windowHeight);

	void Bind()
	{
		if (width != 0 && height != 0)
			glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	}

	void BindWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
	}

	void BindRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
	}

	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetReadBuffer(GBUFFER_TYPE type)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
	}

	void Draw(Quad& quad);

	std::vector<GLuint> textures;

private:
	std::vector<GLuint> attachments;
	GLuint fbo = 0;
	GLuint rbo = 0;

	int width = 0, height = 0;
};