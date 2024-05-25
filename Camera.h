#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaderprogram.h"

class Camera {
	public:
		glm::vec3 position;
		glm::vec3 facing;
		glm::vec3 up;
		glm::vec3 speed;

		bool firstMouse;

		int width;
		int height;

		float sensitivity;
		float yaw, pitch;

		Camera(int width, int height, glm::vec3 position);

		void updateViewMatrix(float fov, float near, float far, ShaderProgram& shader, const char* view = "V", const char* projection = "P");

		void rotate(GLFWwindow* window, float yaw, float pitch);
		void move(double dt);
};

#endif