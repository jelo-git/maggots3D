#include "rocket.h"

#define GRAVITY_ACC 9.8f
#define PROXIMITY 0.5f

Rocket::Rocket(std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices)
{
	this->indices_size = indices.size();
	// Create VAO for rendering
	this->vao = VAO();
	this->vao.Bind();
	// Create VBO for terrain vertices
	VBO vbo = VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
	VBO vbo_normals = VBO(normals.data(), normals.size() * sizeof(GLfloat));
	VBO vbo_texCoords = VBO(texCoords.data(), texCoords.size() * sizeof(GLfloat));
	// Create EBO for terrain indices
	EBO ebo = EBO(indices.data(), indices.size() * sizeof(GLuint));
	// Link VBO to VAO
	this->vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	this->vao.LinkAttrib(&vbo_normals, 1, 3, GL_FLOAT, 0, (void*)0);
	this->vao.LinkAttrib(&vbo_texCoords, 2, 2, GL_FLOAT, 0, (void*)0);
	// Unbind VBO and VAO
	vbo.Unbind();
	vbo_normals.Unbind();
	vbo_texCoords.Unbind();
	this->vao.Unbind();
	ebo.Unbind();
}

void Rocket::spawn(glm::vec3 position, GLfloat speed, GLfloat yaw, GLfloat pitch)
{
	this->position = position;
	this->speed = glm::vec3(speed * glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		speed * glm::sin(glm::radians(pitch)),
		speed * glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)));
	this->yaw = yaw;
	this->pitch = pitch;
}

void Rocket::updatePosition(float deltaTime, glm::vec3 wind)
{
	glm::vec3 acceleration = glm::vec3(wind.x, wind.y - GRAVITY_ACC, wind.z);
	this->speed += acceleration * deltaTime;
	this->position += this->speed * deltaTime;
	this->yaw = glm::atan(this->speed.z, this->speed.x);
	this->pitch = glm::asin(this->speed.y / glm::length(this->speed));
}

bool Rocket::collisionHappened(Player& player)
{
	return (glm::distance(this->position, player.position) < PROXIMITY);
}

bool Rocket::collisionHappened(Terrain& terrain)
{
	return (this->position.y <= terrain.getHeight(this->position.x, this->position.z));
}

void Rocket::draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength)
{
	shader.use();
	camera.shaderMatrix(shader, "V", "P");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position);
	model = glm::rotate(model, -yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, pitch, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

	glUniformMatrix4fv(shader.u("M"), 1, GL_FALSE, glm::value_ptr(model));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4fv(shader.u("lightColor"), 1, glm::value_ptr(lightColor));

	glUniform3fv(shader.u("viewPos"), 1, glm::value_ptr(camera.position));

	glUniform1f(shader.u("expStr"), explLightStrength);
	glUniform3fv(shader.u("lightPos"), 1, glm::value_ptr(explPos));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_base);
	glUniform1i(shader.u("textureBase"), 0);

	this->vao.Bind();
	glDrawElements(GL_TRIANGLES, this->indices_size, GL_UNSIGNED_INT, NULL);
	this->vao.Unbind();
}