#version 410 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

float SampleShadowMap(sampler2D shadowMap, vec2 coords, float compare)
{
	return step(compare, texture(shadowMap, coords.xy).r);
}

float SampleShadowMapLinear(sampler2D shadowMap, vec2 coords, float compare, vec2 texelSize)
{
	vec2 pixelPos = coords/texelSize + vec2(0.5);
	vec2 fracPart = fract(pixelPos);
	vec2 startTexel = (pixelPos - fracPart) * texelSize;

	float blTexel = SampleShadowMap(shadowMap, startTexel, compare);
	float brTexel = SampleShadowMap(shadowMap, startTexel + vec2(texelSize.x, 0.0), compare);
	float tlTexel = SampleShadowMap(shadowMap, startTexel + vec2(0.0, texelSize.y), compare);
	float trTexel = SampleShadowMap(shadowMap, startTexel + texelSize, compare);

	float mixA = mix(blTexel, tlTexel, fracPart.y);
	float mixB = mix(brTexel, trTexel, fracPart.y);

	return mix(mixA, mixB, fracPart.x);
}

float SampleShadowMapPCF(sampler2D shadowMap, vec2 coords, float compare, vec2 texelSize)
{
	const float NUM_SAMPLES = 3.0f;
	const float SAMPLES_START = (NUM_SAMPLES-1.0f)/2.0f;
	const float NUM_SAMPLES_SQUARED = NUM_SAMPLES*NUM_SAMPLES;

	float result = 0.0f;
	for(float y = -SAMPLES_START; y <= SAMPLES_START; y += 1.0f)
	{
		for(float x = -SAMPLES_START; x <= SAMPLES_START; x += 1.0f)
		{
			vec2 coordsOffset = vec2(x,y)*texelSize;
			result += SampleShadowMapLinear(shadowMap, coords + coordsOffset, compare, texelSize);
		}
	}
	return result/NUM_SAMPLES_SQUARED;
}


float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    return 1.0 - SampleShadowMapPCF(shadowMap, projCoords.xy, currentDepth - bias, texelSize);
}

vec3 phong()
{
        vec3 ambient = light.ambient * material.ambient;
        vec3 norm = normalize(fs_in.Normal);
        vec3 lightDir = normalize(light.position - fs_in.FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);
        return ambient + diffuse + specular;
}

void main()
{
    vec3 color = vec3(0.3, 0.9, 0.3);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    vec3 ambient = 0.3 * color;
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 ph = phong();
    vec3 lighting = ((ambient + (1.0 - shadow) * (diffuse + specular)) * ph) + ph;
    FragColor = vec4(lighting, 1.0);
}