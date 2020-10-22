#pragma once

class Object {
public:
	Mesh myMesh;
	glm::vec3 myPos = glm::vec3(0.0f);
	glm::vec3 myRot = glm::vec3(0.0f);
	glm::vec3 mySca = glm::vec3(1.0f);
	glm::mat4 myMat = glm::mat4(1.0f);
	Shader* myShader;

	Object(Mesh mesh, Shader* shader) {
		myMesh = mesh;
		myShader = shader;
	}
	void translate(glm::vec3 trans) {
		myPos += trans;
	}
	void setPos(glm::vec3 pos) {
		myPos = pos;
	}
	void rotateX(float rot) {
		myRot.x = rot;
	}
	void rotateY(float rot) {
		myRot.y = rot;
	}
	void rotateZ(float rot) {
		myRot.z = rot;
	}
	void basicMat() {
		myMat = glm::mat4(1.0f);
		myMat = glm::scale(myMat, mySca);
		myMat = glm::translate(myMat, myPos);
		myMat = glm::rotate(myMat, glm::radians(myRot.x), glm::vec3(1, 0, 0));
		myMat = glm::rotate(myMat, glm::radians(myRot.y), glm::vec3(0, 1, 0));
		myMat = glm::rotate(myMat, glm::radians(myRot.z), glm::vec3(0, 0, 1));
	}
	void swordMat() {
		myMat = glm::mat4(1.0f);
		myMat = glm::scale(myMat, mySca);
		myMat = glm::translate(myMat, myPos);
		myMat = glm::rotate(myMat, glm::radians(myRot.x), glm::vec3(1, 0, 0));
		myMat = glm::rotate(myMat, glm::radians(myRot.z), glm::vec3(0, 0, 1));
		myMat = glm::rotate(myMat, glm::radians(myRot.y), glm::vec3(0, 1, 0));
	}
	void gemMat() {
		myMat = glm::mat4(1.0f);
		myMat = glm::scale(myMat, mySca);
		myMat = glm::rotate(myMat, glm::radians(myRot.x), glm::vec3(1, 0, 0));
		myMat = glm::rotate(myMat, glm::radians(myRot.y), glm::vec3(0, 1, 0));
		myMat = glm::rotate(myMat, glm::radians(myRot.z), glm::vec3(0, 0, 1));
		myMat = glm::translate(myMat, myPos);
	}
	virtual void draw() {
		myShader->use();
		myShader->setMat("model", myMat);
		myMesh.Draw(*myShader);
	}
};
#ifdef OLD

class instancedObj : public Object
{
public:
	glm::mat4* modelMatrix;
	unsigned int VAO;

	instancedObj(Mesh mesh, Shader* shader, glm::mat4* modelMat, glm::mat4 pos = glm::mat4(1.0f)) : Object(mesh, shader, pos) {
		if (modelMat == NULL) {
			std::cout << "failure to get a model matrix for instanced obj" << std::endl;
		}
		modelMatrix = modelMat;
		VAO = mesh.VAO;
	}

	 void draw() {
		myShader->use();
		myShader->setMat("model", myPos);
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::mat4), &modelMatrix[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);

		for (unsigned int i = 0; i < myMesh.Textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			if (myMesh.Textures[i].type == "Spec") {
				glBindTexture(GL_TEXTURE_2D, myMesh.Textures[i].id);
				myShader->setInt("material.specular", i);
			}
			else if (myMesh.Textures[i].type == "Diff") {
				glBindTexture(GL_TEXTURE_2D, myMesh.Textures[i].id);
				myShader->setInt("material.baseTex", i);
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, myMesh.index.size(), GL_UNSIGNED_INT, 0, 100);
		glBindVertexArray(0);
	}
};
glm::mat4* genRandomPos(glm::vec3 center, float radius = 2.0f, int amount = 100, float scale = 1.0f) {
	glm::mat4* modelMatrix;
	modelMatrix = new glm::mat4[amount];
	for (int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		float angle = i / (float)amount * 360.0f;
		float offset = (float)(rand() % (int)(2 * 0.0625 * 100)) / 100.0f - 0.0625f;
		float x = center.x +(sin(angle)* radius + offset);
		offset = (rand() % (int)(2 * 0.0625f * 100)) / 100.0f - 0.0625f;
		float y = center.y + -sin(angle)* radius + offset;
		offset = (rand() % (int)(2 * 0.0625f * 100)) / 100.0f - 0.0625f;
		float z = center.z + cos(angle)* radius + offset;
		model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::scale(model, glm::vec3(scale));
		int test = rand() % 10;
		x = (float)((rand() % 10) / 100.0f);
		y = (float)((rand() % 10) / 100.0f);
		z = (float)((rand() % 10) / 100.0f);
		angle = (float)((rand() % 36000) / 100);
		model = glm::rotate(model, glm::radians(angle), glm::vec3(x, y, z));
		modelMatrix[i] = model;
	}
	return modelMatrix;
};
#endif
