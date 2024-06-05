#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "hitbox.h"

class Player
{
public:
	glm::vec3 position;
	int hp;
	Hitbox hitbox;

	// VAO
	VAO vao;
	GLuint indices_size;

	Player(glm::vec3 startPosition, int hp, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);

	void updateHP(int damage);
};

#endif // !PLAYER_H
