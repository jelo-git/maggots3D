#include "player.h"

Player::Player(glm::vec3 startPosition, int hp, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& texCoords, std::vector<GLuint>& indices)
{
	this->hp = hp;
	this->position = startPosition;

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

void Player::updateHP(int damage)
{
	hp -= damage;
}

void Player::draw(ShaderProgram& shader, Camera& camera, glm::vec3& explPos, float explLightStrength)
{
	shader.use();
	camera.shaderMatrix(shader, "V", "P");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position + glm::vec3(0.0f, 0.25f, 0.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, this->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

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