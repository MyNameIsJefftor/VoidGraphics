#version 330 core
out vec4 FragColor;  
in vec3 Color;

uniform vec3 lightColor;
  
void main()
{
    FragColor = vec4(lightColor * Color, 1.0);
}