#version 460 core
out vec4 FragColor;
  
in vec2 TexCoord;
uniform sampler2D tex;

void main()
{
    vec3 texel = texture(tex, TexCoord).rgb;
    FragColor = vec4(texel, 1);
}