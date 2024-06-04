#include "rocket.h"

#define GRAVITY_ACC 9.8
#define TIME_ACC 10

Rocket::Rocket(GLfloat x, GLfloat y, GLfloat z, const std::vector<GLfloat>& vertices)
{
	this->position = glm::vec3(x, y, z);
	this->hitbox = Hitbox(vertices);
	this->initial_velocity = 0.0;
	this->horizontal_angle = 0.0;
	this->vertical_angle = 0.0;
}

GLfloat Rocket::getSlopeAngle(float time)
{
	float v0y = initial_velocity * glm::sin(glm::radians(vertical_angle)) * glm::sin(glm::radians(horizontal_angle));
	time *= TIME_ACC;
	return v0y - GRAVITY_ACC * time;
}

glm::vec3 Rocket::getMovementCoords(float time)
{
	GLfloat x, y, z;

	time *= TIME_ACC;

	float velocity_x0 = initial_velocity * glm::cos(glm::radians(vertical_angle)) * glm::cos(glm::radians(horizontal_angle));
	float velocity_z0 = initial_velocity * glm::sin(glm::radians(vertical_angle)) * glm::sin(glm::radians(horizontal_angle));
	float velocity_y0 = initial_velocity * glm::sin(glm::radians(vertical_angle));

	x = velocity_x0 * time;
	y = velocity_y0 * time - GRAVITY_ACC * pow(time, 2) / 2;
	z = velocity_z0 * time;

	return glm::vec3(x, y, z) + position;
}

void Rocket::setVelocity(GLfloat initial_velocity)
{
	this->initial_velocity = initial_velocity;
}

void Rocket::setVerticalAngle(GLfloat vertical_angle)
{
	this->vertical_angle = vertical_angle;
}

void Rocket::setHorizontalAngle(GLfloat horizontal_angle)
{
	this->horizontal_angle = horizontal_angle;
}

bool Rocket::collisionHappened(Player& player)
{
	if (this->hitbox.x2 >= player.hitbox.x1 && 
		this->hitbox.y2 >= player.hitbox.y1 && 
		this->hitbox.z2 >= player.hitbox.z1)
		return true;

	return false;
}