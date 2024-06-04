#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class VBO {
public:
	GLuint id;
	VBO(GLfloat* vertices, GLsizeiptr size);
	void Bind();
	void Unbind();
	void Delete();
};

#endif // !VBO_CLASS_H