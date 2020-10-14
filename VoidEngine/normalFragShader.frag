#version 330 core
flat out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;
in vec2 Tex;

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

void main()
{
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 dirFinal[maxDirLights];
    vec3 pointFinal[maxPointLights];
    vec3 norm = normalize(Normal);

    //setup dirlights
    for(int i = 0; i < maxDirLights; i++){
        vec3 lightDir = normalize(-dirlight[i].direction);
        vec3 reflectdir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectdir),0.0), 32);
        vec3 specular = dirlight[i].specular * spec * vec3(texture(material.specular, Tex));
        vec3 diff = max(dot(norm, lightDir), 0.0) * dirlight[i].color* vec3(texture(material.baseTex, Tex));
        dirFinal[i] =  diff + specular;
    }

    //setup pointlights
    for(int i = 0; i<maxPointLights; i ++){
        vec3 lightDir = normalize(pointlight[i].pos - FragPos);
        vec3 reflectdir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectdir),0.0), 32);
        vec3 specular = pointlight[i].specular * vec3(texture(material.specular, Tex)) * spec;
        vec3 diff = max(dot(norm, lightDir),0.0) * pointlight[i].color *vec3(texture(material.baseTex, Tex));
        float dis = length(pointlight[i].pos-FragPos);
        float atten = 1.0 / (pointlight[i].constant + pointlight[i].linear * dis + pointlight[i].quadratic * (dis * dis));
        diff *= atten;
        specular *= atten;
        pointFinal[i] = diff + specular;
    }
    vec3 NewColor = vec3(Color.x - FragPos.x, Color.y - FragPos.y, Color.z - FragPos.z);
    NewColor = mix(NewColor, vec3(0.1,0.1,0.3),sin((Time+FragPos.z)/2));
    vec4 final;
    for(int i = 0; i < numDirLights; i ++){
        if(dirFinal[i] == vec3(0,0,0)){
            continue;
        }
        final += vec4((dirFinal[i]),1.0);
    }
    for(int i = 0; i < numPointLights; i ++){
        final += vec4((pointFinal[i]),1.0);
    }
    FragColor = final;
}