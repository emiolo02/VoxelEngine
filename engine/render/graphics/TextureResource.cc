#include "config.h"
#include "TextureResource.h"
#include "core/stb_image.h"
#include <iostream>

TextureResource* TextureResource::instance = nullptr;

TextureResource* 
TextureResource::Instance()
{
	if (instance == nullptr)
		instance = new TextureResource();

	return instance;
}

TextureData
TextureResource::LoadTexture(std::string path, bool flipVert, GLint filtering)
{
	if (loadedTextures.find(path) != loadedTextures.end())
		return loadedTextures[path]; // Dont load if texture already loaded

	TextureData tex = LoadFromFile(path.c_str(), flipVert, filtering);
	loadedTextures[path] = tex;

	return tex;
}

TextureData
TextureResource::LoadTexture(ImageInfo imInfo, bool flipVert, GLint filtering)
{
	if (loadedTextures.find(imInfo.name) != loadedTextures.end())
		return loadedTextures[imInfo.name]; // Dont load if texture already loaded

	TextureData tex;

	if (imInfo.isBinary)
		tex = LoadFromData(&imInfo.BinaryData[0], imInfo.BinaryData.size());
	else
		tex = LoadFromFile(imInfo.FilePath.c_str(), flipVert, filtering);

	loadedTextures[imInfo.name] = tex;

	return tex;
}

TextureData
TextureResource::LoadCubeMap(std::vector<std::string> paths, GLint filtering)
{
	if (loadedTextures.find(paths[0]) != loadedTextures.end())
		return loadedTextures[paths[0]]; // Dont load if texture already loaded

	TextureData tex;
	tex.target = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex.id);

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		unsigned char* data = stbi_load(paths[i].c_str(), &tex.width, &tex.height, &tex.nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << paths[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return tex;
}

TextureData
TextureResource::LoadFromFile(const char* imagePath, bool flipVert, GLint filtering)
{
	TextureData tex;

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

	// load and generate the texture
	stbi_set_flip_vertically_on_load(flipVert);
	unsigned char* loadedData = stbi_load(imagePath, &tex.width, &tex.height, &tex.nrChannels, STBI_rgb_alpha);
	if (loadedData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.height, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture at \"" << imagePath << "\"" << std::endl;
		return GetDefaultWhite();
	}
	stbi_image_free(loadedData);
	stbi_set_flip_vertically_on_load(false);

	return tex;
}

TextureData
TextureResource::LoadFromData(const unsigned char* data, int size)
{
	TextureData tex;

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels;
	unsigned char* loadedData = stbi_load_from_memory(data, size, &tex.width, &tex.height, &nrChannels, STBI_rgb_alpha);
	if (loadedData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(loadedData);

	return tex;
}

TextureData
TextureResource::LoadColor(const unsigned char* data)
{
	TextureData tex;
	tex.width = 1;
	tex.height = 1;

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	return tex;
}

void
TextureResource::InitDefaults()
{
	const unsigned char colors[3][4] =
	{
	{255, 255, 255, 255}, // white texture
	{128, 128, 255, 0  }, // flat normal
	{0,   0,   0,   0  }  // black texture
	};

	loadedTextures["defaultWhite"] = LoadColor(colors[0]);

	loadedTextures["defaultNormal"] = LoadColor(colors[1]);
	
	loadedTextures["defaultBlack"] = LoadColor(colors[2]);
}

TextureData 
TextureResource::GetDefaultWhite()
{
	if (loadedTextures.find("defaultWhite") == loadedTextures.end())
		InitDefaults();
	
	return loadedTextures["defaultWhite"];
}

TextureData
TextureResource::GetDefaultNormal()
{
	if (loadedTextures.find("defaultNormal") == loadedTextures.end())
		InitDefaults();

	return loadedTextures["defaultNormal"];
}

TextureData
TextureResource::GetDefaultBlack()
{
	if (loadedTextures.find("defaultBlack") == loadedTextures.end())
		InitDefaults();

	return loadedTextures["defaultBlack"];
}