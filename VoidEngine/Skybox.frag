#version 330 core
out vec4 FragColor; 

in SHADER_OUT{
vec3 Color;
vec3 Normal;
vec3 FragPos;
vec2 Tex;
} ShaderIn;

uniform samplerCube skybox;

uniform bool grayScale;

void main()
{

	vec4 fOut = textureCube(skybox, ShaderIn.FragPos)+vec4(0.03);
	if(grayScale){
		float gray = (fOut.x+fOut.y+fOut.z)/3;
		fOut = vec4(vec3(gray),1.0);
	}
	FragColor = fOut;
}