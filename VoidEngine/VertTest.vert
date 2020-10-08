#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTex;

out vec3 Normal;
out vec3 Color;
out vec3 FragPos;
out vec2 Tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool UsePoint;
uniform vec3 lightPos;
uniform vec3 ambiantLightColor;
uniform vec3 lightColor;
uniform sampler2D theTex;
uniform float Time;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(model*vec4(aPos,1.0f));
    Normal = mat3(transpose(inverse(model)))*aNormal;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos-FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff) * lightColor;

    Color = vec3(0.4,0.4,0.4)+diffuse;
    Tex = vec2(aTex.x, aTex.y);
} 