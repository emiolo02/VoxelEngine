#include "config.h"
#include "Material.h"

void
Material::AddTexture(Texture const& tex)
{
	textures.push_back(tex);
}

BlinnPhongMaterial::BlinnPhongMaterial()
{
	this->ambient = vec3(1, 1, 1);
	this->diffuse = vec3(1, 1, 1);
	this->specular = vec3(1, 1, 1);
}

BlinnPhongMaterial::BlinnPhongMaterial(vec3 ambient, vec3 diffuse, vec3 specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void
BlinnPhongMaterial::SetProperties(vec3 ambient, vec3 diffuse, vec3 specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void
BlinnPhongMaterial::Apply(GLuint program)
{
	for (auto& tex : textures)
		tex.Bind();

	//UniformVec3("material.ambient", this->ambient, program);
	UniformVec3("diffuse", this->diffuse, program);
	//UniformVec3("material.specular", this->specular, program);
}