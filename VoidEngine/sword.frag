#version 330 core
out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

struct pointLight{
    vec3 pos;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    vec3 specular;
};

uniform samplerCube reflection;
uniform vec3 camPos;
#define maxPointLights 100
uniform pointLight[maxPointLights] pointlight;
uniform int numPointLights;
uniform vec3 ambient;

uniform bool grayScale;

void main()
{	
    vec3 viewDir = normalize(camPos - ShaderIn.FragPos);
    vec3 pointFinal[maxPointLights];
	vec3 norm = normalize(ShaderIn.Normal);
	vec3 I = normalize(ShaderIn.FragPos - camPos);
	vec3 R = reflect(I, norm);
    //setup pointlights
    for(int i = 0; i<maxPointLights; i ++){
        vec3 lightDir = normalize(pointlight[i].pos - ShaderIn.FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        vec3 reflectdir = reflect(-lightDir, norm);
        float spec = pow(max(dot(norm, halfDir),0.0), 32);
        vec3 specular = pointlight[i].specular * textureCube(reflection, R).rgb * spec;
        vec3 diff = max(dot(norm, lightDir),0.0) * pointlight[i].color * textureCube(reflection, R).rgb;
        float dis = length(pointlight[i].pos-ShaderIn.FragPos);
        float atten = 1.0 / (pointlight[i].constant + pointlight[i].linear * dis + pointlight[i].quadratic * (dis * dis));
        diff *= atten;
        specular *= atten;
        pointFinal[i] = diff + specular;
    }
	vec4 final;
    for(int i = 0; i < numPointLights; i ++){
        final += vec4((pointFinal[i]),1.0);
    }
	if(grayScale){
        float r = final.x;
        float g = final.y;
        float b = final.z;
        float gray = (r+g+b)/3;
		final = vec4(vec3(gray)+ambient, 1.0f);
	}
	FragColor = final + vec4(textureCube(reflection, R).rgb * ambient, 1);
}