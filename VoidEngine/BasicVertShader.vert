#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTex;

out SHADER_OUT {
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    ShaderOut.Color = aColor;
    ShaderOut.Normal = aNormal;
    ShaderOut.Tex = aTex;
    ShaderOut.FragPos = vec3(model*vec4(aPos,1.0f));
} 