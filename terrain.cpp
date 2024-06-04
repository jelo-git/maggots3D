#include "terrain.h"


Terrain::Terrain(int size, float height, int triangleLength, float frequency, int octaves, float persistence)
{
	this->size = size;
	this->triangleLength = triangleLength;
	this->frequency = frequency;
	this->octaves = octaves;
	this->persistence = persistence;
	this->vertices = std::vector<GLfloat>();
	this->indices = std::vector<GLuint>();
	this->normals = std::vector<GLfloat>();
	this->texCoords = std::vector<GLfloat>();
	this->height = height;

	// Generate terrain
	generateTerrain();
	generateIndices();
	generateNormals();
	generateTexCoords();

	// Create VAO for rendering
	this->vao = VAO();
	this->vao.Bind();
	// Create VBO for terrain vertices
	VBO vbo = VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
	// Create VBO for terrain normals
	VBO vboNormals = VBO(normals.data(), normals.size() * sizeof(GLfloat));
	// Create EBO for terrain indices
	EBO ebo = EBO(indices.data(), indices.size() * sizeof(GLuint));
	// Create VBO for terrain texCoords
	VBO vboTexCoords = VBO(texCoords.data(), texCoords.size() * sizeof(GLfloat));
	// Link VBO to VAO
	this->vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	this->vao.LinkAttrib(&vboNormals, 1, 3, GL_FLOAT, 0, (void*)0);
	this->vao.LinkAttrib(&vboTexCoords, 2, 2, GL_FLOAT, 0, (void*)0);
	// Unbind VBO and VAO
	vbo.Unbind();
	vboNormals.Unbind();
	vboTexCoords.Unbind();
	this->vao.Unbind();
	ebo.Unbind();
}

void Terrain::generateTerrain() {
	for (int row = 0; row < size + 1; row++)
	{
		for (int col = 0; col < size + 1; col++)
		{
			glm::vec3 currentVector = glm::vec3(col * triangleLength, row * -triangleLength, perlinNoise(row, col, frequency, octaves, persistence));
			vertices.push_back(currentVector.x);
			vertices.push_back(currentVector.z);
			vertices.push_back(currentVector.y);
		}
	}
}

void Terrain::generateIndices() {
	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			indices.push_back(row * (size + 1) + col); //0
			indices.push_back(row * (size + 1) + col + 1); //1
			indices.push_back((row + 1) * (size + 1) + col); //2

			indices.push_back((row + 1) * (size + 1) + col); //2
			indices.push_back(row * (size + 1) + col + 1); //1
			indices.push_back((row + 1) * (size + 1) + col + 1); //3
		}
	}
}

//generate normals for each vertex moving counter clockwise
void Terrain::generateNormals() {
	for (int i = 0; i < vertices.size(); i += 3)
	{
		normals.push_back(0.0f);
		normals.push_back(0.0f);
		normals.push_back(0.0f);
	}

	for (int i = 0; i < indices.size(); i += 3)
	{
		glm::vec3 v1 = glm::vec3(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
		glm::vec3 v2 = glm::vec3(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
		glm::vec3 v3 = glm::vec3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);

		glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		normals[indices[i] * 3] += normal.x;
		normals[indices[i] * 3 + 1] += normal.y;
		normals[indices[i] * 3 + 2] += normal.z;

		normals[indices[i + 1] * 3] += normal.x;
		normals[indices[i + 1] * 3 + 1] += normal.y;
		normals[indices[i + 1] * 3 + 2] += normal.z;

		normals[indices[i + 2] * 3] += normal.x;
		normals[indices[i + 2] * 3 + 1] += normal.y;
		normals[indices[i + 2] * 3 + 2] += normal.z;
	}

	for (int i = 0; i < normals.size(); i += 3)
	{
		glm::vec3 normal = glm::normalize(glm::vec3(normals[i], normals[i + 1], normals[i + 2]));
		normals[i] = normal.x;
		normals[i + 1] = normal.y;
		normals[i + 2] = normal.z;
	}
}


void Terrain::generateTexCoords() {
	for (int row = 0; row < size + 1; row++)
	{
		for (int col = 0; col < size + 1; col++)
		{
			texCoords.push_back((float)col / size);
			texCoords.push_back((float)row / size);
		}
	}
}

float Terrain::perlinNoise(int x, int y, float frequency, int octaves, float persistence)
{
	float total = 0.0f;
	float amplitude = 1.0f;

	for (int i = 0; i < octaves; i++)
	{
		total += glm::perlin(glm::vec2(x * frequency, y * frequency)) * amplitude;
		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total * height;
}

float Terrain::barycentricInterpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;

	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::getHeight(float x, float y)
{
	int row = (int)(x / triangleLength);
	int col = (int)(y / -triangleLength);

	if (row < 0 || row >= size || col < 0 || col >= size)
	{
		return 0.0f;
	}

	float xCoord = fmod(x, triangleLength) / triangleLength;
	float yCoord = fmod(y, -triangleLength) / -triangleLength;

	float height = 0.0f;
	if (xCoord <= 1 - yCoord)
	{
		height = barycentricInterpolation(
			glm::vec3(0, perlinNoise(row, col, frequency, octaves, persistence), 0),
			glm::vec3(1, perlinNoise(row + 1, col, frequency, octaves, persistence), 0),
			glm::vec3(0, perlinNoise(row, col + 1, frequency, octaves, persistence), 1),
			glm::vec2(xCoord, yCoord)
		);
	}
	else
	{
		height = barycentricInterpolation(
			glm::vec3(1, perlinNoise(row + 1, col, frequency, octaves, persistence), 0),
			glm::vec3(1, perlinNoise(row + 1, col + 1, frequency, octaves, persistence), 1),
			glm::vec3(0, perlinNoise(row, col + 1, frequency, octaves, persistence), 1),
			glm::vec2(xCoord, yCoord)
		);
	}

	return height;
}

void Terrain::draw(ShaderProgram& shader, Camera& camera)
{
	shader.use();
	camera.shaderMatrix(shader, "V", "P");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, -5.0f, 5.0f));

	glUniformMatrix4fv(shader.u("M"), 1, GL_FALSE, glm::value_ptr(model));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4fv(shader.u("lightColor"), 1, glm::value_ptr(lightColor));

	glUniform3fv(shader.u("viewPos"), 1, glm::value_ptr(camera.position));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_base);
	glUniform1i(shader.u("textureBase"), 0);

	this->vao.Bind();
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, NULL);
	this->vao.Unbind();
}