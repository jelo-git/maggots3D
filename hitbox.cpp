#include "hitbox.h"
#include <algorithm>


Hitbox::Hitbox()
{
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;
	z1 = 0;
	z2 = 0;
}

std::vector<glm::vec4> Hitbox::toVec4Vector(const std::vector<GLfloat>& vertices)
{
	std::vector<glm::vec4> vec4Vertices;

	for (int i = 0; i < vertices.size(); i += 3)
	{
		vec4Vertices.push_back(glm::vec4(vertices[i + X], vertices[i + Y], vertices[i + Z], 1));
	}

	return vec4Vertices;
}

std::vector<std::vector<GLfloat>> Hitbox::splitCoords(const std::vector<glm::vec4>& vertices)
{
	std::vector<std::vector<GLfloat>> xyzVectors(3);

	size_t size = vertices.size();

	xyzVectors[X].reserve(size);
	xyzVectors[Y].reserve(size);
	xyzVectors[Z].reserve(size);

	for (auto& vertex : vertices)
	{
		xyzVectors[X].push_back(vertex.x);
		xyzVectors[Y].push_back(vertex.y);
		xyzVectors[Z].push_back(vertex.z);
	}

	return xyzVectors;
}

void Hitbox::updateHitbox(const std::vector<GLfloat>& vertices, const glm::mat4& M, const glm::mat4& V)
{
	std::vector<glm::vec4> vec4Vertices = toVec4Vector(vertices);

	for (auto& vec : vec4Vertices)
		vec = M * V * vec;

	std::vector<std::vector<GLfloat>> splitVertices = splitCoords(vec4Vertices);

	x1 = *std::min_element(splitVertices[X].begin(), splitVertices[X].end());
	x2 = *std::max_element(splitVertices[X].begin(), splitVertices[X].end());
	y1 = *std::min_element(splitVertices[Y].begin(), splitVertices[Y].end());
	y2 = *std::max_element(splitVertices[Y].begin(), splitVertices[Y].end());
	z1 = *std::min_element(splitVertices[Z].begin(), splitVertices[Z].end());
	z2 = *std::max_element(splitVertices[Z].begin(), splitVertices[Z].end());
}

Hitbox::Hitbox(const std::vector<GLfloat>& vertices)
{
	std::vector<glm::vec4> vec4Vertices = toVec4Vector(vertices);
	std::vector<std::vector<GLfloat>> splitVertices = splitCoords(vec4Vertices);

	x1 = *std::min_element(splitVertices[X].begin(), splitVertices[X].end());
	x2 = *std::max_element(splitVertices[X].begin(), splitVertices[X].end());
	y1 = *std::min_element(splitVertices[Y].begin(), splitVertices[Y].end());
	y2 = *std::max_element(splitVertices[Y].begin(), splitVertices[Y].end());
	z1 = *std::min_element(splitVertices[Z].begin(), splitVertices[Z].end());
	z2 = *std::max_element(splitVertices[Z].begin(), splitVertices[Z].end());
}