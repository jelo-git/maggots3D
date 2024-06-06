#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "shaderprogram.h"
#include "camera.h"

class Player
{
public:
	glm::vec3 position;
	GLfloat rotation;
	int hp;

	// VAO
	VAO vao;
	GLuint indices_size;
	// Texture
	GLuint texture_base;

	Player(glm::vec3 startPosition, int hp, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices);

	void updateHP(int damage);

	void draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength);
};

#endif // !PLAYER_H
