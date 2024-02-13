#version 460 core

in vec2 outTexCoord;

uniform sampler2D u_texture;

struct Material
{
	vec3 diffuse;
};

uniform Material material;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(u_texture, outTexCoord) * vec4(material.diffuse, 1);
};