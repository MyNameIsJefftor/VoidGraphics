#version 330 core
out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

void main()
{
    FragColor = vec4(ShaderIn.Color, 1.0);
}