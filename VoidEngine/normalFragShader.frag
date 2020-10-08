#version 330 core
flat out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;
in vec2 Tex;

struct Light {
    vec3 direction;
    vec3 color;
    vec3 ambient;
};

//uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform Light light;
uniform sampler2D theTex;
uniform float Time;
uniform bool UsePoint;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff) * light.color;
    vec3 NewColor = vec3(Color.x - FragPos.x, Color.y - FragPos.y, Color.z - FragPos.z);
    NewColor = mix(NewColor, vec3(0.1,0.1,0.3),sin((Time+FragPos.z)/2));
    vec3 result = light.color+NewColor;
    vec4 final = texture(theTex,Tex)*vec4((0.1+diffuse) * result,1.0);
    vec4 test = texture(theTex,Tex)*vec4((0.1+diffuse) * Color,1.0);
    FragColor = final;
}