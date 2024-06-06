#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>
#include "shaderprogram.h"
#include "camera.h"
#include "VAO.h"
#include "EBO.h"

class Terrain {
private:
	VAO vao;

	float perlinNoise(int x, int y, float frequency, int octaves, float persistence);

	void generateTerrain();
	void generateIndices();
	void generateNormals();
	void generateTexCoords();

	float barycentricInterpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);

public:
	// Terrain properties
	int size, triangleLength;
	float height;
	// Perlin noise properties
	int octaves;
	float frequency, persistence;
	// Terrain data
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texCoords;
	// Texture
	GLuint texture_base;


	Terrain(int size, float height, int triangleLength, float frequency, int octaves, float persistence);

	float getHeight(float x, float y);

	void draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength);
};

#endif