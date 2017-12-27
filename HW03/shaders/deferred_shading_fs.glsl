#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};
uniform int mode;
const int NR_LIGHTS = 128;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
uniform vec3 dir;
uniform int count_l;
uniform float gamma;
void main()
{
    if (mode == 0) {
        vec3 FragPos = texture(gPosition, TexCoords).rgb;
        vec3 Normal = texture(gNormal, TexCoords).rgb;
        vec3 Diffuse = pow(texture(gAlbedoSpec, TexCoords).rgb, vec3(gamma));
        float Specular = texture(gAlbedoSpec, TexCoords).a;
        vec3 lighting  = Diffuse * 0.1;
        vec3 viewDir  = normalize(viewPos - FragPos);
        for(int i = 0; i < count_l; ++i)
        {
            float distance = length(lights[i].Position - FragPos);
            if(distance < lights[i].Radius)
            {
                vec3 lightDir = normalize(lights[i].Position - FragPos);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
                vec3 specular = lights[i].Color * spec * Specular;
                float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
                diffuse *= attenuation;
                specular *= attenuation;
                lighting += (diffuse + specular) * (lights[i].Radius - distance) / lights[i].Radius * 3;
            }
        }
        FragColor = vec4(pow(lighting, vec3(1.0 / gamma)), 1.0);
    }
    else if (mode == 1)
    {
        vec3 FragPos = texture(gPosition, TexCoords).rgb;
        FragColor = vec4(FragPos, 1.0);
    }
    else if (mode == 2)
    {
        vec3 Normal = texture(gNormal, TexCoords).rgb;
        FragColor = vec4(Normal, 1.0);
    }
    else if (mode == 3)
    {
        vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
        FragColor = vec4(Diffuse, 1.0);
    }
}
