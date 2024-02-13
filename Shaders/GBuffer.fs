#version 460 core

in vec3 Position;
in vec2 TexCoord;
in mat3 TBN;
in vec4 lightSpacePos;

layout(binding = 0) uniform sampler2D u_texture;
layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_emissive;
layout(binding = 3) uniform sampler2D u_AO;
layout(binding = 4) uniform sampler2D u_metallicRoughness;

layout(location = 0) out vec4 gAlbedoSpecular;
layout(location = 1) out vec3 gPosition;
layout(location = 2) out vec3 gNormal;
layout(location = 3) out vec3 gLightPos;

uniform vec3 diffuse;

void main()
{
    if (texture(u_texture, TexCoord).a < 0.1)
        discard;

    gPosition = Position;
    gLightPos = lightSpacePos.xyz/lightSpacePos.w;

    if (isnan(TBN[0][0])) // if tangents are 0 just use vertex normals
    {
        gNormal = normalize(TBN[2]);
    }
    else
    {
        gNormal = texture(u_normal, TexCoord).rgb;
	    gNormal = normalize(gNormal * 2 - 1);
	    gNormal = normalize(TBN * gNormal);
    }
    
    vec4 albedo = texture(u_texture, TexCoord); 
    albedo.r *= diffuse.r;
    albedo.g *= diffuse.g;
    albedo.b *= diffuse.b;

    gAlbedoSpecular = 
    albedo * texture(u_AO, TexCoord).r + 
    texture(u_emissive, TexCoord);

    gAlbedoSpecular.a = 1 - texture(u_metallicRoughness, TexCoord).g;
    // gShadowFactor.r = CalcShadow();
}