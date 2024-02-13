#version 460 core

layout(location = 0) in vec3 inPosition;

out vec3 TexCoord;

uniform mat4 invProj;
uniform mat4 invView;

void main()
{
	vec3 viewPos = (invProj * vec4(inPosition, 1)).xyz;

    TexCoord = (mat3(invView) * viewPos);
	gl_Position = vec4(inPosition.xy, 1, 1);
};