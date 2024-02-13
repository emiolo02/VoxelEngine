#version 460 core
out vec4 FragColor;
  
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D gAlbedoSpec;
layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D lightSpacePos;
layout(binding = 4) uniform sampler2D shadowMap;

uniform vec3 cameraPos;


// struct Material
// {
// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// };

// uniform Material material;

struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
	float radius;

	float constant;
	float linear;
	float quadratic;
};

uniform int NUM_LIGHTS;
#define MAX_POINT_LIGHTS 128
uniform PointLight pointLights[MAX_POINT_LIGHTS];

struct Sun
{
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform Sun sun;

float CalcShadow()
{
	vec3 projCoords = texture(lightSpacePos, TexCoord).xyz;
	vec2 UV;
	UV.x = 0.5 * projCoords.x + 0.5;
	UV.y = 0.5 * projCoords.y + 0.5;
	float z = 0.5 * projCoords.z + 0.5;

	float depth = texture(shadowMap, UV).x;
	float bias = 0.0025;

	if (depth + bias < z)
		return 0;
	
	return 1;
}

vec3 CalcSun(vec3 normal, vec3 fragPos, vec3 camPos, vec3 albedo, float spec, float shadowFactor)
{
	// return ambient if in shadow
	if (shadowFactor == 0)
		return vec3(0.1f, 0.1f, 0.1f) * albedo;

	vec3 sunDir = -sun.direction;
	// Diffuse
	vec3 diffuse = sun.color * clamp(dot(sunDir, normal), 0, 1) * sun.intensity;

	// Highlight
	vec3 viewDir = normalize(camPos - fragPos);
	vec3 halfWay = normalize(sunDir + viewDir);
	float specularConstant = pow(max(dot(normal, halfWay), 0), 16);
	vec3 highlight = sun.color * specularConstant;

	
	// diffuse *= shadowFactor;
	// highlight *= shadowFactor;

	return (vec3(0.1f, 0.1f, 0.1f) + diffuse + highlight*spec) * albedo;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 camPos, vec3 albedo, float spec)
{
	// Ambient
	vec3 ambientLight = vec3(0.1f, 0.1f, 0.1f);
	
	// Diffuse
	vec3 dirToLight = normalize(light.position - fragPos);
	vec3 diffuse = light.color * clamp(dot(dirToLight, normal), 0, 1);
	float distance = length(light.position - fragPos);
	diffuse = diffuse * (1.0 / (1.0 + (0.01 * distance))) * light.intensity;


	// Highlight
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 viewDir = normalize(camPos - fragPos);
	vec3 halfWay = normalize(lightDir + viewDir);
	float specularConstant = pow(max(dot(normal, halfWay), 0), 50);
	vec3 highlight = light.color * specularConstant;

	float attenuation = light.intensity / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));   

	ambientLight *= attenuation;
	diffuse *= attenuation;
	highlight *= attenuation;

	return (ambientLight + diffuse + highlight*spec) * albedo;
}

void main()
{ 
    vec3 fragPos = texture(gPosition, TexCoord).xyz;
    vec3 normal = texture(gNormal, TexCoord).xyz;
    vec3 albedo = texture(gAlbedoSpec, TexCoord).xyz;
	float specular = texture(gAlbedoSpec, TexCoord).w;


	float shadowFactor = CalcShadow();
	
    vec3 lighting = vec3(0);

    for (int i = 0; i < NUM_LIGHTS; i++)
	{
		float dist = length(fragPos - pointLights[i].position);
		if (dist < pointLights[i].radius)
		{
        	lighting += CalcPointLight(pointLights[i], normal, fragPos, cameraPos, albedo, specular);
		}
	}

    lighting += CalcSun(normal, fragPos, cameraPos, albedo, specular, shadowFactor);

    FragColor = vec4(lighting, 1);
}