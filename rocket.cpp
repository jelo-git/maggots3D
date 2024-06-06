#include "rocket.h"

#define GRAVITY_ACC 9.8f
#define PROXIMITY 0.5f

Rocket::Rocket(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices)
{
	this->position = glm::vec3(x, y, z);
	this->initial_position = glm::vec3(x, y, z);
	this->initial_velocity = 0.0;
	this->horizontal_angle = 0.0;
	this->vertical_angle = 0.0;
	this->time = 0.0;

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

GLfloat Rocket::getSlopeAngle()
{
	float v0y = initial_velocity * glm::sin(glm::radians(vertical_angle));
	return glm::atan(v0y - GRAVITY_ACC * this->time);
}

glm::vec3 Rocket::getMovementCoords(float deltaTime)
{
	GLfloat x, y, z;
	this->time += deltaTime;

	float velocity_x0 = initial_velocity * glm::cos(glm::radians(vertical_angle)) * glm::cos(glm::radians(horizontal_angle));
	float velocity_z0 = initial_velocity * glm::sin(glm::radians(vertical_angle)) * glm::sin(glm::radians(horizontal_angle));
	float velocity_y0 = initial_velocity * glm::sin(glm::radians(vertical_angle));

	x = velocity_x0 * this->time;
	y = velocity_y0 * this->time - GRAVITY_ACC * pow(this->time, 2) / 2;
	z = velocity_z0 * this->time;

	getSlopeAngle();

	return glm::vec3(x, y, z) + initial_position;
}

void Rocket::setVelocity(GLfloat initial_velocity)
{
	this->initial_velocity = initial_velocity;
}

void Rocket::setVerticalAngle(GLfloat vertical_angle)
{
	this->vertical_angle = vertical_angle;
}

void Rocket::setHorizontalAngle(GLfloat horizontal_angle)
{
	this->horizontal_angle = horizontal_angle;
}

bool Rocket::collisionHappened(Player& player)
{
	return (glm::distance(this->position, player.position) < PROXIMITY);
}

bool Rocket::collisionHappened(Terrain& terrain)
{
	return (this->position.y <= terrain.getHeight(this->position.x, this->position.z));
}

void Rocket::collisionEvent()
{
	this->time = 0.0;
}

void Rocket::draw(ShaderProgram& shader, Camera& camera)
{
	shader.use();
	camera.shaderMatrix(shader, "V", "P");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position);
	model = glm::rotate(model, this->getSlopeAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

	glUniformMatrix4fv(shader.u("M"), 1, GL_FALSE, glm::value_ptr(model));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4fv(shader.u("lightColor"), 1, glm::value_ptr(lightColor));

	glUniform3fv(shader.u("viewPos"), 1, glm::value_ptr(camera.position));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_base);
	glUniform1i(shader.u("textureBase"), 0);

	this->vao.Bind();
	glDrawElements(GL_TRIANGLES, this->indices_size, GL_UNSIGNED_INT, NULL);
	this->vao.Unbind();
}