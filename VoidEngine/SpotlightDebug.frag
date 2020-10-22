#version 330 core
flat out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

struct mat{
    sampler2D baseTex;
    sampler2D specular;
};
struct spotLight {
	vec3 color;
	vec3 specular;
	vec3 direction;
	vec3 pos;
	float cutOff;
	float cutOffOut;
    float constant;
    float linear;
    float quadratic;
};
struct dirLight {
    vec3 direction;
    vec3 color;
    vec3 specular;
};

struct pointLight{
    vec3 pos;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    vec3 specular;
};

#define maxDirLights 100
#define maxPointLights 100
#define maxSpotLights 100
uniform dirLight[maxDirLights] dirlight;
uniform int numDirLights;
uniform pointLight[maxPointLights] pointlight;
uniform int numPointLights;
uniform spotLight[maxSpotLights] spotlight;
uniform int numSpotLights;
uniform float Time;
uniform bool UsePoint;
uniform mat material;
uniform vec3 camPos;

uniform bool grayScale;

void main()
{
    spotLight spot =  {
    vec3(1.0),
    vec3(0.5),
    vec3(0,1,0),
    vec3(0,2,0),
    cos(radians(12.5)),
    cos(radians(15.0)),
    1,
    0.09,
    0.032
    };
    
    vec3 viewDir = normalize(camPos - ShaderIn.FragPos);
    vec3 norm = normalize(ShaderIn.Normal);

    vec3 lightDir = normalize(spotlight[0].pos - ShaderIn.FragPos);
    // diffuse shading
    float diff = max(dot(norm, spotlight[0].direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(spotlight[0].pos - ShaderIn.FragPos);
    float attenuation = 1.0 / (spotlight[0].constant + spotlight[0].linear * distance + spotlight[0].quadratic * (distance * distance));    
    // spotlight intensity
    float cutIn = cos(radians(spotlight[0].cutOff));
    float cutOut = cos(radians(spotlight[0].cutOffOut));
    float theta = dot(lightDir, normalize(spotlight[0].direction)); 
    float epsilon = cutIn - cutOut;
    float intensity = clamp((theta - cutOut) / epsilon, 0.0, 1.0);
    // combine results
    //vec3 ambient = vec3(0.3) * texture(material.baseTex, ShaderIn.Tex).rgb;
    vec3 diffuse = spotlight[0].color * diff * texture(material.baseTex, ShaderIn.Tex).rgb;
    vec3 specular = spotlight[0].specular * spec * texture(material.specular, ShaderIn.Tex).rgb;
    //ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    FragColor = vec4((diffuse + specular),1.0);
}