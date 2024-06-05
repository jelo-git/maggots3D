#include "player.h"

Player::Player(glm::vec3 startPosition, int hp, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices)
{
	this->hp = hp;
	this->position = startPosition;
	this->hitbox = Hitbox(vertices);

	this->indices_size = indices.size();
	// Create VAO for rendering
	this->vao = VAO();
	this->vao.Bind();
	// Create VBO for terrain vertices
	VBO vbo = VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
	// Create EBO for terrain indices
	EBO ebo = EBO(indices.data(), indices.size() * sizeof(GLuint));
	// Link VBO to VAO
	this->vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 0, (void*)0);
	// Unbind VBO and VAO
	vbo.Unbind();
	this->vao.Unbind();
	ebo.Unbind();
}

void Player::updateHP(int damage)
{
	hp -= damage;
}