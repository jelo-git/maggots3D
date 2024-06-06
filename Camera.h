#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaderprogram.h"

class Camera {
private:
	float zoom;
public:
	glm::vec3 position;
	glm::vec3 facing;
	glm::vec3 up;

	//movement speed
	glm::vec3 speed;

	glm::mat4 view;
	glm::mat4 projection;

	//window size
	int width;
	int height;

	//mouse controls
	float sensitivity;
	float yaw, pitch;
	bool firstMouse;

	Camera(int width, int height, glm::vec3 position);

	void updateMatrix(float fov, float near, float far);
	void shaderMatrix(ShaderProgram& shader, const char* view = "V", const char* projection = "P");

	void followPlayer(glm::vec3 playerPos);
	void rotateAroundPlayer(glm::vec3 playerPos, float yaw, float pitch);
	void setZoom(float zoom);

	void rotate(GLFWwindow* window, float yaw, float pitch);
	void move(double dt);
};

#endif