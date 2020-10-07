#version 330 core
out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;
in vec2 Tex;

uniform vec3 ambiantLightColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D theTex;
uniform float Time;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos-FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 NewColor = vec3(Color.x - FragPos.x, Color.y - FragPos.y, Color.z - FragPos.z);
    NewColor = mix(NewColor, vec3(0.1,0.1,0.3),sin((Time+FragPos.z)/2));
    vec3 result = ambiantLightColor+NewColor;
    vec4 final = texture(theTex,Tex)*vec4(diffuse + result,1.0);
    FragColor = final;
}