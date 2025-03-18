#version 450 core

uniform sampler2D diffuse;

in vec3 normal;
in vec2 uv;

out vec4 FragColor;

const vec3 sunDirection = normalize(vec3(0.3, -1, 0.3));

void main()
{
    const vec4 diffuseSample = texture(diffuse, uv);
    if (diffuseSample.a < 0.1) discard;

    const vec3 color = diffuseSample.rgb;
    const float sunStrength = max(0.2, dot(-sunDirection, normal));
    FragColor = vec4(color * sunStrength, 1.0);
}