#ifndef ROCKET_H
#define ROCKET_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "shaderprogram.h"
#include "camera.h"
#include "VAO.h"
#include "EBO.h"
#include "terrain.h"

#include "player.h"

class Rocket
{
public:
	glm::vec3 position;
	glm::vec3 initial_position;

	GLfloat initial_velocity;
	GLfloat vertical_angle;
	GLfloat horizontal_angle;
	GLfloat time;

	// VAO
	VAO vao;
	GLuint indices_size;
	// Texture
	GLuint texture_base;


	Rocket(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices);

	glm::vec3 getMovementCoords(float deltaTime);
	GLfloat getSlopeAngle();
	void setVelocity(GLfloat initial_velocity);
	void setVerticalAngle(GLfloat vertical_angle);
	void setHorizontalAngle(GLfloat horizontal_angle);
	bool collisionHappened(Player& player);
	bool collisionHappened(Terrain& terrain);
	void collisionEvent();

	void draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength);
};

#endif // !ROCKET_H
