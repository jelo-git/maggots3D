#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef PHYSICS_H
#define PHYSICS_H

#define GRAVITY_ACC 9.8
#define MAGIC_NUMBER 17

glm::vec3 getMovementCoords(float initial_velocity, float alpha, float beta, float time, const glm::vec3& start)
{
	GLfloat x, y, z;
	
	time *= MAGIC_NUMBER;

	float velocity_x0 = initial_velocity * glm::cos(glm::radians(alpha)) * glm::cos(glm::radians(beta));
	float velocity_z0 = initial_velocity * glm::sin(glm::radians(alpha)) * glm::sin(glm::radians(beta));
	float velocity_y0 = initial_velocity * glm::sin(glm::radians(alpha));

	x = velocity_x0 * time;
	y = velocity_y0 * time - GRAVITY_ACC * pow(time, 2) / 2;
	z = velocity_z0 * time;
	
	return glm::vec3(x, y, z) + start;
}

#endif // !PHYSICS_H
