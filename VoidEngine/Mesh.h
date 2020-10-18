#pragma once
#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec3 Normal;
	glm::vec2 TexCord;
};

struct Texture {
	unsigned int id;
	std::string type;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> index;
	std::vector<unsigned int> TextureId;//0 is diffuse, 1 is specular
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind, std::vector<unsigned int> text);
	void Draw(Shader& shader);
	void DrawnUnIndex(Shader& shader);
	//Render Data
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), &index[0], GL_STATIC_DRAW);

	//vertex Pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//vertex color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

	//vertex Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	//vertex Texture
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCord));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
	/*shader.setInt("material.baseTex", TextureId[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId[0]);
	if (TextureId[1] != 0) {
		shader.setInt("material.specular", TextureId[1]);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, TextureId[1]);
	}*/

	for (unsigned int i = 0; i < TextureId.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		if (i == 1) {
			glBindTexture(GL_TEXTURE_2D, TextureId[i]);
			shader.setInt("material.specular", i);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, TextureId[i]);
			shader.setInt("material.baseTex", i);
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::DrawnUnIndex(Shader& shader) {
	/*shader.setInt("material.baseTex", TextureId[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId[0]);
	if (TextureId[1] != 0) {
		shader.setInt("material.specular", TextureId[1]);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, TextureId[1]);
	}*/

	for (unsigned int i = 0; i < TextureId.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		if (i == 1) {
			glBindTexture(GL_TEXTURE_2D, TextureId[i]);
			shader.setInt("material.specular", i);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, TextureId[i]);
			shader.setInt("material.baseTex", i);
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind,std::vector<unsigned int> text) {
	this->vertices = verts;
	this->index = ind;
	this->TextureId = text;

	setupMesh();
}

Mesh CreateCube(std::vector<unsigned int> tex, float scale = 1, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f)) {
	std::vector<Vertex> cube = {
		//pos										//color		//normal							//TexCord
	//1
		{glm::vec3(-0.5f,  0.5f, -0.5f) * scale,	color,		glm::vec3(-0.33f, 0.33f, -0.33f),	glm::vec2(0.0f, 0.0f)},
	//2												
		{glm::vec3(-0.5f,  0.5f, 0.5f) * scale,		color,		glm::vec3(-0.33f, 0.33f, 0.33f),	glm::vec2(0.0f, 1.0f)},
	//3												
		{glm::vec3(0.5f,  0.5f, 0.5f) * scale,		color,		glm::vec3(0.33f, 0.33f, 0.33f),		glm::vec2(1.0f, 1.0f)},
	//4												
		{glm::vec3(0.5f,  0.5f, -0.5f) * scale,		color,		glm::vec3(0.33f, 0.33f, -0.33f),	glm::vec2(1.0f, 0.0f)},
	//5												
		{glm::vec3(-0.5f,  -0.5f, -0.5f) * scale,	color,		glm::vec3(-0.33f, -0.33f, -0.33f),	glm::vec2(1.0f, 0.0f)},
	//6												
		{glm::vec3(-0.5f,  -0.5f, 0.5f) * scale,	color,		glm::vec3(-0.33f, -0.33f, 0.33f),	glm::vec2(1.0f, 1.0f)},
	//7												
		{glm::vec3(0.5f,  -0.5f, 0.5f) * scale,		color,		glm::vec3(0.33f, -0.33f, 0.33f),	glm::vec2(0.0f, 1.0f)},
	//8												
		{glm::vec3(0.5f,  -0.5f, -0.5f) * scale,	color,		glm::vec3(0.33f, -0.33f, -0.33f),	glm::vec2(0.0f, 0.0f)}
	};
	std::vector<unsigned int> index = {
		//top
		0, 1, 3,
		1, 2, 3,
		//front
		1, 2, 6,
		1, 5, 6,
		//right
		3, 2, 6,
		3, 7, 6,
		//left
		1, 0, 4,
		1, 5, 4,
		//bottom
		6, 5, 4,
		6, 7, 4,
		//back
		3, 0, 4,
		3, 7, 4
	};
	return Mesh(cube, index, tex);
}

Mesh generatePlane(std::vector<unsigned int> tex, float scale = 1, glm::vec3 Color = glm::vec3(0.5f, 0.5f, 0.5f)) {
	//create verts
	std::vector<Vertex> Plane;
	std::vector<unsigned int> index;
	int i = 0;
	for (float z = 0.4f; z > -0.5f; z -= 0.1f) {
		for (float x = -0.4f; x < 0.5f; x += 0.1f) {
			Vertex temp;
			temp.Position = glm::vec3(x, 0.0f, z)*scale;
			temp.Color = glm::vec3(0.5f, 0.5f, 0.5f);
			temp.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			temp.TexCord = glm::vec2(x*2, z*2);
			Plane.push_back(temp);
		}
	}

	//create index
	for (int ind = 0, cor = 0; cor < 89; cor++) {
		if ((cor + 1) % 10 == 0) {
			continue;
		}
		index.push_back(cor);
		index.push_back(cor + 1);
		index.push_back(cor + 11);

		index.push_back(cor);
		index.push_back(cor + 10);
		index.push_back(cor + 11);
	}
	return Mesh(Plane, index, tex);
}