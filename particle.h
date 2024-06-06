#ifndef PARTICLE_CLASS_H
#define PARTICLE_CLASS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "VAO.h"
#include "EBO.h"
#include "shaderprogram.h"
#include "camera.h"

struct ParticleInfo
{
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 velocityVariation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 startColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 endColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float mass = 1.0f;
	float ttl = 1.0f;
	float size = 1.0f;
	float sizeEnd = 0.0f;
};

class ParticleSystem
{
private:
	// Particle structure
	struct Particle
	{
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		float mass = 1.0f;
		float ttl = 1.0f;
		float size = 1.0f;
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	// Particle pool
	std::vector<Particle> particles;

	// VAO for rendering
	VAO vao;
public:
	ParticleInfo info;

	ParticleSystem(ParticleInfo& info);

	void update(float dt, glm::vec3 extForce = glm::vec3(0.0f, 0.0f, 0.0f));

	void emit(int count);

	void render(ShaderProgram& shader, Camera& camera);
};
#endif // !PARTICLE_CLASS_H