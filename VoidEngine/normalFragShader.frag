#version 330 core
flat out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;
in vec2 Tex;

struct dirLight {
    vec3 direction;
    vec3 color;
};

struct pointLight{
    vec3 pos;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

#define maxDirLights 100
#define maxPointLights 100
uniform dirLight[maxDirLights] dirlight;
uniform int numDirLights;
uniform pointLight[maxPointLights] pointlight;
uniform int numPointLights;
uniform sampler2D theTex;
uniform float Time;
uniform bool UsePoint;
uniform vec3 ambient;

void main()
{
    
    vec3 dirFinal[maxDirLights];
    vec3 pointFinal[maxPointLights];
    vec3 norm = normalize(Normal);

    //setup dirlights
    for(int i = 0; i < maxDirLights; i++){
        vec3 lightDir = normalize(-dirlight[i].direction);
        float diff = max(dot(norm, lightDir), 0.0);
        dirFinal[i] = (diff) * dirlight[i].color;
    }

    //setup pointlights
    for(int i = 0; i<maxPointLights; i ++){
        vec3 lightDir = normalize(pointlight[i].pos - FragPos);
        float diff = max(dot(norm, lightDir),0.0);
        float dis = length(pointlight[i].pos-FragPos);
        float atten = 1.0 / (pointlight[i].constant + pointlight[i].linear * dis + pointlight[i].quadratic * (dis * dis));
        pointFinal[i] = atten * (diff * pointlight[i].color);
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
    final *= texture(theTex,Tex);
    FragColor = final;
}