#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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
    ShaderOut.Color = ShaderIn[0].Color;
    ShaderOut.Normal = ShaderIn[0].Normal;
    ShaderOut.Tex = ShaderIn[0].Tex;
    ShaderOut.FragPos = ShaderIn[0].FragPos;
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.0, 0.1, 0.0, 0.0);
    EmitVertex();

    ShaderOut.Color = ShaderIn[1].Color;
    ShaderOut.Normal = ShaderIn[1].Normal;
    ShaderOut.Tex = ShaderIn[1].Tex;
    ShaderOut.FragPos = ShaderIn[1].FragPos;
    gl_Position = gl_in[1].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4( 0.0, 0.1, 0.0, 0.0);
    EmitVertex();
    ShaderOut.Color = ShaderIn[2].Color;
    ShaderOut.Normal = ShaderIn[2].Normal;
    ShaderOut.Tex = ShaderIn[2].Tex;
    ShaderOut.FragPos = ShaderIn[2].FragPos;
    gl_Position = gl_in[2].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[2].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position + vec4( 0.0, 0.1, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
} 