#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "hitbox.h"
#include "player.h"

#ifndef ROCKET_H
#define ROCKET_H

class Rocket
{
public:
	glm::vec3 position;
	Hitbox hitbox;
	GLfloat initial_velocity;
	GLfloat vertical_angle;
	GLfloat horizontal_angle;

	Rocket(GLfloat x, GLfloat y, GLfloat z, const std::vector<GLfloat>& vertices);

	glm::vec3 getMovementCoords(float time);
	GLfloat getSlopeAngle(float time);
	void setVelocity(GLfloat initial_velocity);
	void setVerticalAngle(GLfloat vertical_angle);
	void setHorizontalAngle(GLfloat horizontal_angle);
	bool collisionHappened(Player& player);
	void collisionEvent();
};

#endif // !ROCKET_H
