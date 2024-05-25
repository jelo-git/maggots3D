#include "camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	this->position = position;
	this->facing = glm::vec3(0.0f, 0.0f, -1.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->width = width;
	this->height = height;
	this->sensitivity = 10.0f;
	this->yaw = 180.0f;
	this->pitch = 0.0f;
	this->firstMouse = true;
	this->speed = glm::vec3(0.0f);
}

void Camera::updateViewMatrix(float fov, float near, float far, ShaderProgram &shader, const char* view, const char* projection) {
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 P = glm::mat4(1.0f);

	V = glm::lookAt(position, position + facing, up);
	P = glm::perspective(glm::radians(fov), (float)(width / height), near, far);

	glUniformMatrix4fv(shader.u(view), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(shader.u(projection), 1, false, glm::value_ptr(P));
}

void Camera::rotate(GLFWwindow* window, float yaw, float pitch) {
	if (this->firstMouse) { 
		this->firstMouse = false;
		return; 
	}
	glm::vec3 direction;

	this->yaw += yaw/sensitivity;
	this->pitch += pitch / sensitivity;
	if (this->pitch > 89.0f) this->pitch = 89.0f;
	else if (this->pitch < -89.0f) this->pitch = -89.0f;

	direction.x = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	direction.y = sin(glm::radians(this->pitch));
	direction.z = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	facing = glm::normalize(direction);
}

void Camera::move(double dt) {
	position += speed.x * facing * (float) dt;
	position += speed.y * up * (float)dt;
	position += speed.z * glm::normalize(glm::cross(facing, up)) * (float)dt;
}