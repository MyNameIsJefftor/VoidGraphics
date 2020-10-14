#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

in SHADER_OUT {
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn[];

out SHADER_OUT {
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderOut;

void main() {
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();
    EndPrimitive();
}