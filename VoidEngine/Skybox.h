#pragma once
#include "Mesh.h"

struct Skybox {
	Mesh cube;
	Texture tex;
	Shader shad;
};

Skybox CreateSkybox(std::string box, Shader shad) {
	ktxTexture* TexHolder;
	KTX_error_code result;
	unsigned int id = 0;
	GLenum target, glerror;

	result = ktxTexture_CreateFromNamedFile(box.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &TexHolder);
	glGenTextures(1, &id);
	result = ktxTexture_GLUpload(TexHolder, &id, &target, &glerror);
	Texture outTex;
	outTex.id = id;
	outTex.ktxTex = TexHolder;
	outTex.type = "CubeMap";
	std::vector<Texture> temp;
	temp.push_back(outTex);
	Mesh cube = CreateCube(temp);
	Skybox out = { cube, outTex, shad};
	return out;
}

void DrawSkybox(Skybox sky, glm::mat4 view, glm::mat4 proj) {
	glDepthMask(GL_FALSE);
	sky.shad.setMat("view", view);
	sky.shad.setMat("projection", proj);
	sky.shad.use();
	glBindVertexArray(sky.cube.VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky.tex.id);
	glDrawElements(GL_TRIANGLES, sky.cube.index.size(), GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);
}
