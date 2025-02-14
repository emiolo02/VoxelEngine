#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_UV;

out vec2 uv;

void main()
{
    uv = a_Position.xy;

    gl_Position = vec4(a_Position, 1.0);
}
