#version 460 core

uniform sampler2D objectTexture;

in vec2 uv;

out vec4 FragColor;

void main()
{
    FragColor = texture(objectTexture, uv);
}