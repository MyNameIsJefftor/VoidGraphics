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
uniform dirLight[maxDirLights] dirlight;
uniform int numDirLights;
uniform pointLight[maxPointLights] pointlight;
uniform int numPointLights;
uniform float Time;
uniform bool UsePoint;
uniform vec3 ambient;
uniform mat material;
uniform vec3 camPos;

uniform bool grayScale;

void main()
{
    vec3 viewDir = normalize(camPos - ShaderIn.FragPos);
    vec3 dirFinal[maxDirLights];
    vec3 pointFinal[maxPointLights];
    vec3 norm = normalize(ShaderIn.Normal);
    if(texture(material.baseTex, ShaderIn.Tex).a < 0.1){
        discard;
    }
    //setup dirlights
    for(int i = 0; i < maxDirLights; i++){
        vec3 lightDir = normalize(-dirlight[i].direction);
        vec3 halfDir = normalize(lightDir + viewDir);
        vec3 reflectdir = reflect(-lightDir, norm);
        float spec = pow(max(dot(norm, halfDir),0.0), 32);
        vec3 specular = dirlight[i].specular * spec * vec3(texture(material.specular, ShaderIn.Tex));
        vec3 diff = max(dot(norm, lightDir), 0.0) * dirlight[i].color* vec3(texture(material.baseTex, ShaderIn.Tex));
        dirFinal[i] =  diff + specular;
    }

    //setup pointlights
    for(int i = 0; i<maxPointLights; i ++){
        vec3 lightDir = normalize(pointlight[i].pos - ShaderIn.FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        vec3 reflectdir = reflect(-lightDir, norm);
        float spec = pow(max(dot(norm, halfDir),0.0), 32);
        vec3 specular = pointlight[i].specular * vec3(texture(material.specular, ShaderIn.Tex)) * spec;
        vec3 diff = max(dot(norm, lightDir),0.0) * pointlight[i].color *vec3(texture(material.baseTex, ShaderIn.Tex));
        float dis = length(pointlight[i].pos-ShaderIn.FragPos);
        float atten = 1.0 / (pointlight[i].constant + pointlight[i].linear * dis + pointlight[i].quadratic * (dis * dis));
        diff *= atten;
        specular *= atten;
        pointFinal[i] = diff + specular;
    }
    vec3 NewColor = vec3(ShaderIn.Color.x - ShaderIn.FragPos.x, ShaderIn.Color.y - ShaderIn.FragPos.y, ShaderIn.Color.z - ShaderIn.FragPos.z);
    NewColor = mix(NewColor, vec3(0.1,0.1,0.3),sin((Time+ShaderIn.FragPos.z)/2));
    vec4 final;
    for(int i = 0; i < numDirLights; i ++){
        final += vec4((dirFinal[i]),1.0);
    }
    for(int i = 0; i < numPointLights; i ++){
        final += vec4((pointFinal[i]),1.0);
    }
    if(grayScale){
        float r = final.x;
        float g = final.y;
        float b = final.z;
        float gray = (r+g+b)/3;
        final = vec4(vec3(gray), 1.0f);
    }
    FragColor = final;
}