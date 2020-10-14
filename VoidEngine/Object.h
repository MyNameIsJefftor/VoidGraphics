#pragma once

struct Object {
	Mesh myMesh;
	glm::mat4 myPos = glm::mat4(1.0f);
};

glm::mat4* genRandomPos(glm::vec3 center, float radius = 2.0f, int amount = 100, float scale = 1.0f) {
	glm::mat4* modelMatrix;
	modelMatrix = new glm::mat4[amount];
	for (int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		glm::translate(model, center);
		float angle = i / (float)amount * 360.0f;
		float offset = (rand() % (int)(2 * 0.0625 * 100)) / 100.0f - 0.0625;
		float x = sin(angle)* radius + offset;
		offset = (rand() % (int)(2 * 0.0625 * 100)) / 100.0f - 0.0625;
		float y = -sin(angle)* radius + offset;
		offset = (rand() % (int)(2 * 0.0625 * 100)) / 100.0f - 0.0625;
		float z = cos(angle)* radius + offset;
		model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::scale(model, glm::vec3(scale));
		modelMatrix[i] = model;
	}
	return modelMatrix;
};

void drawInstancedObj(glm::mat4* modelMatrix, Object original) {
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::mat4), &modelMatrix[0], GL_STATIC_DRAW);

	glBindVertexArray(original.myMesh.VAO);
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

	glBindVertexArray(original.myMesh.VAO);
	glDrawElementsInstanced(GL_TRIANGLES, original.myMesh.index.size(), GL_UNSIGNED_INT, 0, 100);
}