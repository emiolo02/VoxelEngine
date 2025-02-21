// Basic render ignores all textures & lighting
#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

uniform mat4 projView;
uniform mat4 model;

void main()
{
    gl_Position = projView * model * vec4(inPosition, 1.0);
}