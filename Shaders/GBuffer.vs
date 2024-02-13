#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out vec3 Position;
out vec2 TexCoord;
out mat3 TBN;
out vec4 lightSpacePos;

uniform mat4 model;
uniform mat4 projView;
uniform mat4 lightView;

void main()
{
	Position = (model * vec4(inPosition, 1)).xyz;
	TexCoord = inTexCoord;

	// chop off the 4th row to remove the translation
	mat3 model3 = mat3(model);

	vec3 T = normalize(model3 * inTangent.xyz);
	vec3 N = normalize(model3 * inNormal);
	vec3 B = normalize(cross(N, T))*inTangent.w;

	TBN = mat3(T, B, N);

	lightSpacePos = lightView * vec4(Position, 1);

	gl_Position = projView * model * vec4(inPosition, 1);
};