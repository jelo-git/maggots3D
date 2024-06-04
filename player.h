#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "hitbox.h"

class Player
{
public:
	const GLuint damage = 100;

	glm::vec3 position;
	GLuint hp;
	bool is_active;
	Hitbox hitbox;

	Player(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& vertices);

	//void attack(GLfloat rocket_initial_velocity, GLfloat vertical_angle, GLfloat horizontal_angle);
	void move(glm::vec2& direction, std::vector<GLfloat>& plane, const int div);
	void updateHP();
	void newTurn();

private:
	float areaOfTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
	bool pointIsInsideTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y);
	float getZCoordInsideTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec2& t, float norm_zcoord);
};

#endif // !PLAYER_H
