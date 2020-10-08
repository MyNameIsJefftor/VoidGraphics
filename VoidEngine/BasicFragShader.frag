#version 330 core
out vec4 FragColor;  
in vec3 Color;
  in vec3 Normal;
  in vec3 FragPos;
  in vec2 Tex;
void main()
{
    FragColor = vec4(Color, 1.0);
}