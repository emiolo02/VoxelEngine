#include "config.h"
#include "FrameBuffer.h"

void
Quad::Init()
{
	float vertices[] =
	{
		/* pos         UV */
		-1, -1, 0,    0, 0,
		 1, -1, 0,    1, 0,
		 1,  1, 0,    1, 1,
		-1,  1, 0,    0, 1
	};

	int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// Upload vertex data
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * 4, (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * 4, (GLvoid*)12);


	/*////   Setup ibo     ////*/
	glGenBuffers(1, &quadIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void 
Quad::Bind()
{
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
}


void 
FrameBuffer::Init(int windowWidth, int windowHeight, int buffers)
{
	textures.resize(buffers);
	attachments.resize(buffers);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i = 0; i < textures.size(); i++)
	{
		GLuint& texture = textures[i];

		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture, 0);
	}

	glDrawBuffers(buffers, &attachments[0]);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FRAMEBUFFER ERROR - " << status << std::endl;
		return;
	}
}

void FrameBuffer::InitShadowBuffer(int width, int height)
{
	this->width = width;
	this->height = height;
	textures.resize(1);

	glGenFramebuffers(1, &fbo);

	glGenTextures(1, &textures[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::InitGBuffer(int windowWidth, int windowHeight)
{
	textures.resize(GBUFFER_NUM_TEXTURES);
	attachments.resize(GBUFFER_NUM_TEXTURES);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// - color + specular color buffer
	glGenTextures(1, &textures[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);

	// - position color buffer
	glGenTextures(1, &textures[1]);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[1], 0);

	// - normal color buffer
	glGenTextures(1, &textures[2]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures[2], 0);

	glGenTextures(1, &textures[3]);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, textures[3], 0);


	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = 
	{ 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2, 
		GL_COLOR_ATTACHMENT3
	};
	glDrawBuffers(4, attachments);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FRAMEBUFFER ERROR - " << status << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
FrameBuffer::Draw(Quad& quad)
{
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}

	quad.Bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}