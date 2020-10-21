#version 330 core
out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

uniform samplerCube skybox;

void main()
{
	FragColor = textureCube(skybox, ShaderIn.FragPos)+vec4(0.03);

}