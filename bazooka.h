#ifndef BAZOOKA_H
#define BAZOOKA_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "shaderprogram.h"
#include "camera.h"

class Bazooka
{
public:
	glm::vec3 position;
	GLfloat yaw, pitch;

	// VAO
	VAO vao;
	GLuint indices_size;
	// Texture
	GLuint texture_base;

	Bazooka(glm::vec3 startPosition, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices);

	void draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength);
};

#endif // !PLAYER_H
