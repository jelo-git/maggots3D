#include "particle.h"

ParticleSystem::ParticleSystem(ParticleInfo& info)
{
	ParticleInfo partinfo;
	partinfo.position = info.position;
	partinfo.velocity = info.velocity;
	partinfo.velocityVariation = info.velocityVariation;
	partinfo.mass = info.mass;
	partinfo.ttl = info.ttl;
	partinfo.size = info.size;
	partinfo.sizeEnd = info.sizeEnd;
	partinfo.gravity = info.gravity;
	partinfo.startColor = info.startColor;
	partinfo.endColor = info.endColor;


	this->info = partinfo;

	// create VAO for rendering
	this->vao = VAO();
	this->vao.Bind();
	// create VBO for particle position
	float vertices[] = {
	-0.1f, -0.1f, 0.0f,
	0.1f, -0.1f, 0.0f,
	0.1f, 0.1f, 0.0f,
	-0.1f, 0.1f, 0.0f,
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,
	};
	VBO vbo = VBO(vertices, sizeof(vertices));
	EBO ebo = EBO(indices, sizeof(indices));
	// link VBO to VAO
	this->vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	// unbind VBO and VAO
	vbo.Unbind();
	this->vao.Unbind();
	ebo.Unbind();
}

void ParticleSystem::update(float dt, glm::vec3 extForces)
{
	for (int i = particles.size() - 1; i >= 0; i--)
	{
		// update particle position
		particles[i].position += particles[i].velocity * dt;
		// apply gravity
		particles[i].velocity += this->info.gravity * dt;
		// apply external forces
		particles[i].velocity += extForces * this->info.mass * dt;
		// apply size 
		particles[i].size = this->info.size + (this->info.sizeEnd - this->info.size) * (1.0f - particles[i].ttl / this->info.ttl);
		// apply color
		particles[i].color = this->info.startColor + (this->info.endColor - this->info.startColor) * (1.0f - particles[i].ttl / this->info.ttl);
		// decrease particle ttl
		particles[i].ttl -= dt;
		// remove particle if ttl is less than 0
		if (particles[i].ttl <= 0.0f)
		{
			particles.erase(particles.begin() + i);
		}
	}
}

void ParticleSystem::emit(int count)
{
	for (int i = 0; i < count; i++)
	{
		Particle part;
		part.position = this->info.position;
		part.velocity = this->info.velocity + glm::vec3(
			(this->info.velocityVariation.x * 2.0f) * ((rand() % 100) / 100.0f - 0.5f),
			(this->info.velocityVariation.y * 2.0f) * ((rand() % 100) / 100.0f - 0.5f),
			(this->info.velocityVariation.z * 2.0f) * ((rand() % 100) / 100.0f - 0.5f)
		);
		part.mass = this->info.mass;
		part.ttl = this->info.ttl;
		part.size = this->info.size;
		particles.push_back(part);
	}
}

void ParticleSystem::render(ShaderProgram& shader, Camera& camera)
{
	shader.use();
	camera.shaderMatrix(shader, "V", "P");
	for (int i = 0; i < particles.size(); i++)
	{
		// render particle
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, particles[i].position);
		model = glm::scale(model, glm::vec3(particles[i].size));
		model = glm::rotate(model, glm::radians(camera.yaw - 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(shader.u("M"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3fv(shader.u("partColor"), 1, glm::value_ptr(particles[i].color));
		this->vao.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		this->vao.Unbind();
	}
}