// Basic render ignores all textures & lighting
#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 projView;
uniform mat4 model;

out vec3 normal;
out vec2 uv;

void main()
{
    normal = inNormal;
    uv = inTexCoord;
    gl_Position = projView * model * vec4(inPosition, 1.0);
}