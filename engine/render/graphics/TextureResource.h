#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "core/app.h"

enum TexType
{
	COLOR,
	NORMAL,
	EMMISIVE,
	OCCLUSION,
	METALLIC_ROUGHNESS,

	INVALID
};

struct ImageInfo
{
	std::string FilePath;
	std::string name;

	std::vector<uint8_t> BinaryData;

	bool isBinary = false;
private:
};

struct TextureData
{
	GLuint id = 0;
	TexType type = COLOR;
	GLenum target = GL_TEXTURE_2D;
	int width = 0, height = 0, nrChannels = 0;
};

class TextureResource
{
public:


	static TextureResource* Instance();
	
	TextureData LoadTexture(std::string path, bool flipVert = true, GLint filtering = GL_LINEAR);
	TextureData LoadTexture(ImageInfo imInfo, bool flipVert = true, GLint filtering = GL_LINEAR);
	TextureData LoadCubeMap(std::vector<std::string> paths, GLint filtering = GL_LINEAR);

	TextureData GetDefaultWhite();
	TextureData GetDefaultNormal();
	TextureData GetDefaultBlack();

private:
	static TextureResource* instance;

	void InitDefaults();

	TextureData LoadFromFile(const char* imagePath, bool flipVert, GLint filtering);
	TextureData LoadFromData(const unsigned char* data, int size);
	TextureData LoadColor(const unsigned char* data);

	std::unordered_map<std::string, TextureData> loadedTextures;
	TextureResource() {}
};

class Texture
{
public:
	Texture(TextureData textureData)
	{
		data = textureData;
	}

	Texture()
	{
		data = TextureData();
	}

	void Delete() { glDeleteTextures(1, &data.id); }

	void Bind()
	{
		if (data.target == GL_TEXTURE_2D)
			glActiveTexture(GL_TEXTURE0 + data.type);
		glBindTexture(data.target, this->data.id);
	}

	void UnBind()
	{
		glActiveTexture(GL_TEXTURE0 + data.type);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	TextureData data;
};