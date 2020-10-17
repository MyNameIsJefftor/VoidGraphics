#pragma once
enum CameraMov {
	Forward,
	Backward,
	Left,
	Right
};

class Camera {
public:
	glm::vec3 pos, up, front, right, WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraSpace;

	float yaw = -90.0f, pitch = 0.0f, MoveSpeed = 2.5f, Sens = 0.1f;
	Camera() {};
	Camera(glm::vec3 npos, glm::vec3 nup, glm::vec3 nfront);
	void keyboardInput(CameraMov dir, float deltaTime);
	void mouseInput(float xoff, float yoff);
	glm::mat4 getViewMat();
protected:
	void updateCamVec();
};
glm::mat4 Camera::getViewMat() {
	return glm::lookAt(pos, pos + front, up);
}
Camera::Camera(glm::vec3 npos,glm::vec3 nup, glm::vec3 nfront) {
	pos = npos;
	up = nup;
	front = nfront;
	right = glm::normalize(glm::cross(front, WorldUp));
	cameraSpace = glm::lookAt(pos, pos+front, up);
}
void Camera::keyboardInput(CameraMov dir, float deltaTime) {
	float velo = MoveSpeed * deltaTime;
	if (dir == Forward) {
		pos += (velo * front);
	}
	if (dir == Left) {
		pos -= right * velo;
	}
	if (dir == Backward) {
		pos -= (velo * front);
	}
	if (dir == Right) {
		pos += right * velo;
	}
}

void Camera::mouseInput(float xoff, float yoff) {
	xoff *= Sens;
	yoff *= Sens;

	yaw += xoff;
	pitch += yoff;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
	updateCamVec();
}

void Camera::updateCamVec() {
	glm::vec3 nfront;
	nfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	nfront.y = sin(glm::radians(pitch));
	nfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(nfront);

	right = glm::normalize(glm::cross(front, WorldUp));
	up = glm::normalize(glm::cross(right, front));
}