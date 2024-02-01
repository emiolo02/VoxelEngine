#pragma once

#include "TextureResource.h"
#include "ShaderResource.h"
#include <memory>

class Material
{
public:
	Material() {}

	virtual void Apply(GLuint program) = 0;

	void AddTexture(Texture const& tex);

protected:

	std::vector<Texture> textures;
};

class BlinnPhongMaterial : public Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
public:
	BlinnPhongMaterial();
	BlinnPhongMaterial(vec3 ambient, vec3 diffuse, vec3 specular);
	void SetProperties(vec3 ambient, vec3 diffuse, vec3 specular);
	void Apply(GLuint program) override;
};