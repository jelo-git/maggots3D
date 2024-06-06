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
	glm::vec3 speed;
	GLfloat yaw;
	GLfloat pitch;

	// VAO
	VAO vao;
	GLuint indices_size;
	// Texture
	GLuint texture_base;

	Rocket(std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices);

	void spawn(glm::vec3 position, GLfloat speed, GLfloat yaw, GLfloat pitch);
	void updatePosition(float deltaTime, glm::vec3 wind = glm::vec3(0.0f, 0.0f, 0.0f));

	bool collisionHappened(Player& player);
	bool collisionHappened(Terrain& terrain);

	void draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength);
};

#endif // !ROCKET_H
