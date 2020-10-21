#pragma once

struct Texture {
	unsigned int id;
	ktxTexture* ktxTex;
	std::string type;
};


Texture CreateDiff(std::string Diff) {
	ktxTexture* TexHolder;
	KTX_error_code result;
	unsigned int id = 0;
	GLenum target, glerror;

	result = ktxTexture_CreateFromNamedFile(Diff.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &TexHolder);
	glGenTextures(1, &id);
	result = ktxTexture_GLUpload(TexHolder, &id, &target, &glerror);
	Texture outTex;
	outTex.id = id;
	outTex.ktxTex = TexHolder;
	outTex.type = "Diff";
	return outTex;
}

Texture CreateSpec(std::string Spec) {
	ktxTexture* TexHolder;
	KTX_error_code result;
	unsigned int id = 0;
	GLenum target, glerror;

	result = ktxTexture_CreateFromNamedFile(Spec.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &TexHolder);
	glGenTextures(1, &id);
	result = ktxTexture_GLUpload(TexHolder, &id, &target, &glerror);
	Texture outTex;
	outTex.id = id;
	outTex.ktxTex = TexHolder;
	outTex.type = "Spec";
	return outTex;
}