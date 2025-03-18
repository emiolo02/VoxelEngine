#version 450 core
layout (location = 0) in float dummy;

uniform mat4 projView;
uniform vec4 v0pos;
uniform vec4 v1pos;
uniform vec4 color;

out vec4 outColor;

void main()
{
    outColor = color;
    if (gl_VertexID == 0)
    {
        gl_Position = projView * v0pos;
    }
    else
    {
        gl_Position = projView * v1pos;
    }
}