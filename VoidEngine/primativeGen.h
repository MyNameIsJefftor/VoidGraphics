#pragma once
#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
};

class cube {
public:
	float verts[48] = {
			//Pos				//normal
		//1
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,	
		//2	    
			-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//3	    
			0.5f ,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//4	    
			0.5f ,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
		//5
			-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f,
		//6	   
			-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//7	   
			0.5f , -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//8	   
			0.5f , -0.5f, -0.5f, 0.5f, 0.5f, 0.5f
		};
	unsigned int index[36] = {
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
};
class testTriangle {
public:
	float verts[18] = {
		// positions			// colors
		 0.000f,  0.1f, 0.0f,	1.0f, 0.0f, 0.0f,   // top
		-0.125f, -0.1f, 0.0f,	0.0f, 1.0f, 0.0f,   // left
		 0.125f, -0.1f, 0.0f,	0.0f, 0.0f, 1.0f    // right 
	};
	testTriangle() {

	};
	testTriangle(float color[], float scale = 1.0f) {
		verts[0] *= scale;
		verts[1] *= scale;
		verts[2] *= scale;

		verts[3] = color[0];
		verts[4] = color[1];
		verts[5] = color[2];

		verts[6] *= scale;
		verts[7] *= scale;
		verts[8] *= scale;

		verts[9] = color[0];
		verts[10] = color[1];
		verts[11] = color[2];

		verts[12] *= scale;
		verts[13] *= scale;
		verts[14] *= scale;

		verts[15] = color[0];
		verts[16] = color[1];
		verts[17] = color[2];
	}
};
class genericTerrain {
public:
	float verts[600] = { 0.0f, };
	unsigned int index[486] = { 0, };
	genericTerrain(float color[]) {
		//create vertex
		int vertex = 0;
			for (float z = 0.4f; z > -0.5f; z -= 0.1f) {
				for (float x = -0.4f; x < 0.5f; x += 0.1f) {
					verts[vertex] = x; //X
					verts[vertex + 1] = ((rand() % 20) - 10) / 1000.0f; //random y
					verts[vertex + 2] = z; //z
					//color
					verts[vertex + 3] = color[0];
					verts[vertex + 4] = color[1];
					verts[vertex + 5] = color[2];
					/* uncomment when we add texture
					verts[vertex + 6] = uv[0];
					verts[vertex + 7] = uv[1];
					*/
					vertex += 6;
				}
			}
		//create index
		for (int ind = 0, cor = 0; cor < 89; cor++) {
			if ((cor + 1) % 10 == 0) {
				cor++;
			}
			index[ind++] = cor;
			index[ind++] = cor + 1;
			index[ind++] = cor + 11;

			index[ind++] = cor;
			index[ind++] = cor + 10;
			index[ind++] = cor + 11;
			
		}
	}
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> index;

	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind);
	void Draw(Shader& shader);
private:
	//Render Data
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind) {
	this->vertices = verts;
	this->index = ind;

	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1 , &VAO);
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

	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}