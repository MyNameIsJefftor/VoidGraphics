#version 330 core
out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

uniform vec3 camPos;

void main()
{
    vec3 viewDir = normalize(camPos - ShaderIn.FragPos);
    FragColor = vec4(vec3(ShaderIn.Color * dot(viewDir, ShaderIn.Normal)),1.0);
}