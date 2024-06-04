#include "player.h"

#define MAX_HP 300

Player::Player(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& vertices)
{
	this->hp = MAX_HP;
	this->is_active = false;
	this->position = glm::vec3(x, y, z);
	this->hitbox = Hitbox(vertices);
}

void Player::newTurn()
{
	is_active = true;
}

void Player::updateHP()
{
	hp = hp - damage;
}

//void Player::attack(GLfloat rocket_initial_velocity, GLfloat vertical_angle, GLfloat horizontal_angle)
//{
//	if (!is_active)
//		return;
//
//	// ToDo
//}

void Player::move(glm::vec2& direction, std::vector<GLfloat>& plane, const int div)
{
	float zcoord = 0.0;
	position += glm::vec3(direction, 0);
	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;

			if (pointIsInsideTriangle(plane[3 * index], plane[3 * index + 1],
				plane[3 * (index + (div + 1))], plane[3 * (index + (div + 1)) + 1],
				plane[3 * (index + (div + 1) + 1)], plane[3 * (index + (div + 1) + 1) + 1],
				position.x, position.y))
			{
				glm::vec3 v1(plane[3 * (index + (div + 1))] - plane[3 * index],
					plane[3 * (index + (div + 1)) + 1] - plane[3 * index + 1],
					plane[3 * (index + (div + 1)) + 2] - plane[3 * index + 2]);

				glm::vec3 v2(plane[3 * (index + (div + 1) + 1)] - plane[3 * index],
					plane[3 * (index + (div + 1) + 1) + 1] - plane[3 * index + 1],
					plane[3 * (index + (div + 1) + 1) + 2] - plane[3 * index + 2]);

				glm::vec2 t(position.x - plane[3 * index], position.y - plane[3 * index + 1]);

				zcoord = getZCoordInsideTriangle(v1, v2, t, position.z - plane[3 * index + 2]);
				position += glm::vec3(0, 0, zcoord);
				return;
			}
			else if (pointIsInsideTriangle(plane[3 * index], plane[3 * index + 1],
				plane[3 * (index + 1)], plane[3 * (index + 1) + 1],
				plane[3 * (index + (div + 1) + 1)], plane[3 * (index + (div + 1) + 1) + 1],
				position.x, position.y))
			{
				glm::vec3 v1(plane[3 * (index + (div + 1) + 1)] - plane[3 * index],
					plane[3 * (index + (div + 1) + 1) + 1] - plane[3 * index + 1],
					plane[3 * (index + (div + 1) + 1) + 2] - plane[3 * index + 2]);

				glm::vec3 v2(plane[3 * (index + 1)] - plane[3 * index],
					plane[3 * (index + 1) + 1] - plane[3 * index + 1],
					plane[3 * (index + 1) + 2] - plane[3 * index + 2]);

				glm::vec2 t(position.x - plane[3 * index], position.y - plane[3 * index + 1]);

				zcoord = getZCoordInsideTriangle(v1, v2, t, position.z - plane[3 * index + 2]);
				position += glm::vec3(0, 0, zcoord);
				return;
			}
		}
	}
}

float Player::areaOfTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

bool Player::pointIsInsideTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y)
{
	float A = areaOfTriangle(x1, y1, x2, y2, x3, y3);
	float A1 = areaOfTriangle(x, y, x2, y2, x3, y3);
	float A2 = areaOfTriangle(x1, y1, x, y, x3, y3);
	float A3 = areaOfTriangle(x1, y1, x2, y2, x, y);
	return (A == A1 + A2 + A3);
}

float Player::getZCoordInsideTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec2& t, float norm_zcoord)
{
	glm::vec3 norm = glm::cross(v1, v2);

	return norm_zcoord + (norm.x * t.x + norm.y * t.y) / norm.z;
}