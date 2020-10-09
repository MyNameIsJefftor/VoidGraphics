#pragma once

struct dirLight {
	glm::vec3 direction;
	glm::vec3 color;
};

struct pointLight {
	glm::vec3 position;
	glm::vec3 color;

	float constant;
	float linear;
	float quadratic;
};