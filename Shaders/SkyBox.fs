#version 460 core

in vec3 TexCoord;

layout(binding = 0) uniform samplerCube SkyBox;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(SkyBox, TexCoord);
};