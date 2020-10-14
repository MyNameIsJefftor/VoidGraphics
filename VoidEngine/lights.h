#pragma once

struct dirLight {
	glm::vec3 direction;
	glm::vec3 color;
	glm::vec3 specular;
};

struct pointLight {
	glm::vec3 position;
	glm::vec3 color;

	float constant;
	float linear;
	float quadratic;
	glm::vec3 specular;
};