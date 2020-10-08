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

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(model*vec4(aPos,1.0f));
    Normal = mat3(transpose(inverse(model)))*aNormal;
    Color = aColor;
    Tex = vec2(aTex.x, aTex.y);
} 