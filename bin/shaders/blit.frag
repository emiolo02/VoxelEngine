#version 460 core

in vec2 uv;
uniform sampler2D u_Image;

out vec4 frag;

void main()
{
   frag = texture(u_Image, uv);
}