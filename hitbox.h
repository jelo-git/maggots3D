#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef HITBOX_H
#define HITBOX_H

#define X 0
#define Y 1
#define Z 2

class Hitbox
{
public:
	GLfloat x1, x2;
	GLfloat y1, y2;
	GLfloat z1, z2;
	
	Hitbox();
	Hitbox(const std::vector<GLfloat>& vertices);

	void updateHitbox(const std::vector<GLfloat>& vertices, const glm::mat4& M, const glm::mat4& V);

private:
	std::vector<glm::vec4> toVec4Vector(const std::vector<GLfloat>& vertices);
	std::vector<std::vector<GLfloat>> splitCoords(const std::vector<glm::vec4>& vertices);
};

#endif // !HITBOX_H
